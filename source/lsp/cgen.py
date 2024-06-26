from __future__ import annotations
from dataclasses import dataclass
from typing import List
import re

@dataclass
class BlockComment:
    lines: List[str]

@dataclass
class TypeAnnotation:
    name: str
    cname: str
    is_array: bool

null_type = TypeAnnotation("null", "null", False)

@dataclass 
class FieldUnion:
    comment: BlockComment
    name: str
    is_optional: bool
    is_nulllable: bool
    annotations: List[TypeAnnotation]

@dataclass
class FieldStruct:
    comment: BlockComment
    name: str
    is_optional: bool
    fields: Field

@dataclass
class FieldSingle:
    comment: BlockComment
    name: str
    is_optional: bool
    is_nulllable: bool
    annotation: TypeAnnotation

Field = FieldUnion | FieldStruct | FieldSingle

@dataclass
class Interface:
    parent: str
    name: str
    fields: List[Field]

with open("source/lsp/input.ts", "r") as f:
    text = f.read()

text = text.replace(":", " : ").replace(";", " ; ")
tokens = text.split()

tab = "    "

token_stream = {
    "tokens": tokens,
    "index": 0
}

camel_case_re = re.compile('((?<=[a-z0-9])[A-Z]|(?!^)[A-Z](?=[a-z]))')
def to_snake_case(input):
    return camel_case_re.sub(r'_\1', input.replace("?", "")).lower()

def accept(pattern, token_stream):
    index = token_stream["index"]
    token = token_stream["tokens"][index]
    if re.fullmatch(pattern, token):
        token_stream["index"] = index + 1
        return token
    else:
        return None

def expect(pattern, token_stream):
    token = accept(pattern, token_stream)
    if token is not None:
        return token
    else:
        index = token_stream["index"]
        token = token_stream["tokens"][index]
        message = "Expected token matching pattern " + pattern + " but found " + token
        raise RuntimeError(message)

def parse_block_comment(token_stream):
    if accept("/[*]+", token_stream) is None:
        return None
    
    lines = [""]
    while accept("[*]+/", token_stream) is None:
        if accept("[*]", token_stream) is not None:
            lines[-1] = lines[-1].strip()
            lines.append("")
        else:
            text = expect(".+", token_stream) + " "
            lines[-1] = lines[-1] + text
    lines[-1] = lines[-1].strip()
    return BlockComment(lines)

def parse_field_struct(comment, token_stream, name, is_optional):
    fields = parse_fields(token_stream)
    return FieldStruct(comment, name, is_optional, fields)

def parse_fields(token_stream):
    fields = []
    while accept("}", token_stream) is None:
        fields.append(parse_field(token_stream))
    return fields

def get_type(token_stream):
    type_token = expect("[a-zA-Z]+(\[\])?", token_stream)
    is_array = False
    if "[]" in type_token:
        type_token = type_token.replace("[]", "")
        is_array = True

    if type_token == "uinteger":
        cname = "uint32_t"
    elif type_token == "integer":
        cname = "int32_t"
    elif type_token == "string":
        cname = "string_t"
    elif type_token == "null":
        cname = "null"
    elif type_token == "object":
        cname = "lsp_object"
    elif type_token == "array":
        cname = "lsp_array"
    elif type_token.startswith("LSP"):
        cname = to_snake_case(type_token)
        type_token = cname.replace("lsp_", "")
    elif type_token[0].isupper():
        type_token = to_snake_case(type_token)
        cname = "lsp_" + type_token
    else:
        raise RuntimeError("Unexpected type: " + type_token)
    
    return TypeAnnotation(type_token, cname, is_array)

def parse_field_union(comment, token_stream, first_type, name, is_optional):
    types = [first_type]
    while True:
        types.append(get_type(token_stream))
        if accept("[|]", token_stream) is None:
            break

    
    is_nullable = null_type in types
    if is_nullable:
        types.remove(null_type)

    if len(types) == 1:
        return FieldSingle(comment, name, is_optional, is_nullable, types[0])
    else:
        return FieldUnion(comment, name, is_optional, is_nullable, types)

def parse_field(token_stream):
    comment = parse_block_comment(token_stream)
    name_full = expect("[a-zA-Z]+[?]?", token_stream)
    is_optional = "?" in name_full
    name = to_snake_case(name_full)
    expect(":", token_stream)

    if accept("{", token_stream) is not None:
        field = parse_field_struct(comment, token_stream, name, is_optional)
    else:
        ctype = get_type(token_stream)
        if accept("[|]", token_stream) is not None:
            field = parse_field_union(comment, token_stream, ctype, name, is_optional)
        else:
            field = FieldSingle(comment, name, is_optional, False, ctype)

    expect(";", token_stream)
    return field

def parse_interface(token_stream):
    accept("extern", token_stream)
    expect("interface", token_stream)
    name = "lsp_" + to_snake_case(expect("[a-zA-Z]+", token_stream))

    if accept("extends", token_stream) is not None:
        parent = "lsp_" + to_snake_case(expect("[a-zA-Z]+", token_stream))
    else:
        parent = None
    expect("{", token_stream)
    fields = parse_fields(token_stream)
    return Interface(parent, name, fields)

def ctype_to_name(ctype):
    if ctype == "char *":
        return "string"
    elif "*" in ctype:
        return ctype.replace("*", "").strip()
    else:
        return ctype.strip()
    
def print_block_comment(comment, num_tabs):
    if comment is None:
        return
    print((tab * num_tabs) + "/**", end='')
    first = True
    for line in comment.lines:
        if first:
            first = False
        else:
            print((tab * num_tabs) + " * ", end='')

        print(line)
    print((tab * num_tabs) + " */")

def print_fields(fields, num_tabs, prefix):
    first = True
    for field in fields:
        if first:
            first = False
        else:
            print()

        match field:
            case FieldSingle(comment, name, is_optional, is_nullable, annotation):
                print_block_comment(comment, num_tabs)
                ctype = annotation.cname
                if annotation.is_array:
                    ctype = ctype + " *"
                else:
                    ctype = ctype + " "

                if is_optional:
                    print((tab * num_tabs) + "struct {")
                    print((tab * (num_tabs + 1)) + "bool is_set;")
                    if annotation.is_array:
                        print((tab * (num_tabs + 1)) + "uint32_t length;")
                    print((tab * (num_tabs + 1)) + ctype + "value;")
                    print((tab * num_tabs) + "} " + name + ";")
                else:
                    print((tab * num_tabs) + ctype + name + ";")
            case FieldStruct(comment, name, is_optional, fields):
                print_block_comment(comment, num_tabs)
                print((tab * num_tabs) + "struct {")
                if is_optional:
                    print((tab * (num_tabs + 1)) + "bool is_set;")
                print_fields(fields, num_tabs + 1, prefix + "_" + name)
                print((tab * num_tabs) + "} " + name + ";")
            case FieldUnion(comment, name, is_optional, is_nullable, annotations):
                print_block_comment(comment, num_tabs)
                print((tab * num_tabs) + "struct {")
                if is_optional:
                    print((tab * (num_tabs + 1)) + "bool is_set;")
                print((tab * (num_tabs + 1)) + "enum {")
                for annotation in annotations:
                    print((tab * (num_tabs + 2)) + prefix.upper() + "_" + name.upper() + "_" + annotation.name.upper() + ",")
                if is_nullable:
                    print((tab * (num_tabs + 2)) + prefix.upper() + "_" + name.upper() + "_" + "_NULL,")
                print((tab * (num_tabs + 1)) + "} tag;")
                print((tab * (num_tabs + 1)) + "union {")
                for annotation in annotations:
                    ctype = annotation.cname
                    if annotation.is_array:
                        # TODO need to deal with arrays
                        print((tab * (num_tabs + 2)) + "struct {")
                        print((tab * (num_tabs + 3)) + "uint32_t length;")
                        print((tab * (num_tabs + 3)) + ctype + " *elements;")
                        print((tab * (num_tabs + 2)) + "} " + annotation.name + ";")
                    else:
                        ctype = ctype + " "
                        print((tab * (num_tabs + 2)) + ctype + annotation.name + ";")
                print((tab * (num_tabs + 1)) + "} value;")
                print((tab * num_tabs) + "} " + name + ";")

interface = parse_interface(token_stream)
print("typedef struct {")
if interface.parent is not None:
    print(tab + interface.parent + " super;")
print_fields(interface.fields, 1, interface.name)
print("} " + interface.name + ";")

def print_union_parser(name, annotations, index, num_tabs, prefix):
    if index == len(annotations):
        print((tab * num_tabs) + "return error_message;")
        return
    
    annotation = annotations[index]
    print((tab * num_tabs) + "error_message = accept_" + annotation.name + "(json, \"" + name + "\", &message->" + name + ", NULL);")
    print((tab * num_tabs) + "if(error_message.tag == LSP_PARSE_SUCCESS) {")
    print((tab * (num_tabs + 2)) + "message->" + name + ".tag` = " + prefix.upper() + "_" + annotation.name.upper() + ";")
    print((tab * num_tabs) + "} else {")
    print_union_parser(name, annotations, index + 1, num_tabs + 1, prefix)
    print((tab * num_tabs) + "}")

def print_parser(fields, num_tabs, prefix):
    for field in fields:
        match field:
            case FieldSingle(comment, name, is_optional, is_nullable, annotation):
                field_name = name + "_field"
                print((tab * num_tabs) + "const cJSON *" + field_name + ";")
                print((tab * num_tabs) + "error_message = find_field(json, \"" + name + "\", " + str(is_optional).lower() + ", &" + field_name + ");")
                if is_optional:
                    print((tab * num_tabs) + "if (error_message.tag == LSP_PARSE_SUCCESS_NOT_FOUND) {")
                    print((tab * (num_tabs + 1)) + "message->" + name + ".is_set = false;")
                    print((tab * (num_tabs + 1)) + "goto end_of_" + name + ";")
                    print((tab * num_tabs) + "} else if (error_message.tag != LSP_PARSE_SUCCESS) {")
                    print((tab * (num_tabs + 1)) + "return error_message;")
                    print((tab * num_tabs) + "}")
                    print((tab * num_tabs) + "message->" + name + ".is_set = true;")
                else:
                    print((tab * num_tabs) + "if (error_message.tag != LSP_PARSE_SUCCESS) {")
                    print((tab * (num_tabs + 1)) + "return error_message;")
                    print((tab * num_tabs) + "}")

                if is_optional:
                    struct_field_name = name + ".value"
                else:
                    struct_field_name = name
                print((tab * num_tabs) + "error_message = accept_" + annotation.name + "(" + field_name + ", false, &message->" + struct_field_name + ");")
                print((tab * num_tabs) + "if (error_message.tag != LSP_PARSE_SUCCESS) {")
                print((tab * (num_tabs + 1)) + "return error_message;")
                print((tab * num_tabs) + "}")
                if is_optional:
                    print((tab * num_tabs) + "end_of_" + name + ":")
                print()
            case FieldUnion(comment, name, is_optional, is_nullable, annotations):
                prefix_name = prefix.upper() + "_" + name.upper()
                field_name = name + "_field"
                print((tab * num_tabs) + "const cJSON *" + field_name + ";")
                print((tab * num_tabs) + "error_message = find_field(json, \"" + name + "\", " + str(is_optional).lower() + ", &" + field_name + ");")
                if is_optional:
                    print((tab * num_tabs) + "if (error_message.tag == LSP_PARSE_SUCCESS_NOT_FOUND) {")
                    print((tab * (num_tabs + 1)) + "message->" + name + ".is_set = false;")
                    print((tab * (num_tabs + 1)) + "goto end_of_" + name + ";")
                    print((tab * num_tabs) + "} else if (error_message.tag != LSP_PARSE_SUCCESS) {")
                    print((tab * (num_tabs + 1)) + "return error_message;")
                    print((tab * num_tabs) + "}")
                    print((tab * num_tabs) + "message->" + name + ".is_set = true;")
                else:
                    print((tab * num_tabs) + "if (error_message.tag != LSP_PARSE_SUCCESS) {")
                    print((tab * (num_tabs + 1)) + "return error_message;")
                    print((tab * num_tabs) + "}")
                
                for annotation in annotations:
                    print((tab * num_tabs) + "error_message = accept_" + annotation.name + "(" + field_name + ", false, &message->" + name + ".value." + annotation.name + ");")
                    print((tab * num_tabs) + "if(error_message.tag == LSP_PARSE_SUCCESS) {")
                    print((tab * (num_tabs + 2)) + "message->" + name + ".tag = " + prefix_name + "_" + annotation.name.upper() + ";")
                    print((tab * (num_tabs + 2)) + "goto end_of_" + name + ";")
                    print((tab * num_tabs) + "}")
                print((tab * num_tabs) + "return error_message;")
                print((tab * num_tabs) + "end_of_" + name + ":")
                print()
                    

print("lsp_parse_error_t parse_" + interface.name + "(cJSON *json, " + interface.name + " *message) {")
print(tab + "lsp_parse_error_t error_message;")
print()
print(tab + "parse_" + interface.parent + "(json, &message->super);")
print()
print_parser(interface.fields, 1, interface.name)
print(tab + "return (lsp_parse_error_t){")
print((tab * 2) + ".tag = LSP_PARSE_SUCCESS,")
print((tab * 2) + ".message = NULL,")
print(tab + "};")
print("}")