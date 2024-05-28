#include <common.h>
#include <flecs.h>
#include <parser.h>
#include <span.h>
#include <stdlib.h>

// for use when dfs traversals are implemented
// typedef struct {
//     cortecs_span_t absolute_offset;
// } cortecs_parser_context_t;

// static void *cortecs_parser_context_create(ecs_world_t *world, uint64_t group_id, void *group_by_arg) {
//     UNUSED(world);
//     CORTECS_UNUSED(group_id);
//     CORTECS_UNUSED(group_by_arg);
//     return calloc(1, sizeof(cortecs_parser_context_t));
// }

// static void cortecs_parser_context_destroy(ecs_world_t *world, uint64_t group_id, void *context, void *group_by_arg) {
//     CORTECS_UNUSED(world);
//     CORTECS_UNUSED(group_id);
//     CORTECS_UNUSED(group_by_arg);
//     free(context);
// }

// static int cortecs_parser_span_compare(ecs_entity_t left, const void *left_span, ecs_entity_t right, const void *right_span) {
//     CORTECS_UNUSED(left);
//     CORTECS_UNUSED(right);
//     return cortecs_span_compare(*(cortecs_span_t *)left_span, *(cortecs_span_t *)right_span);
// }

// int main() {
//     ecs_query_desc_t desc = {
//         .filter.terms[0] = (ecs_term_t){
//             .id = ecs_id(cortecs_span_t),
//             .inout = EcsIn,
//         },
//         .filter.terms[1] = (ecs_term_t){
//             .id = ecs_id(cortecs_span_t),
//             .src.flags = EcsUp,
//             .src.trav = EcsChildOf,
//         },
//         .group_by_id = EcsChildOf,
//         .on_group_create = cortecs_parser_context_create,
//         .on_group_delete = cortecs_parser_context_destroy,
//         .order_by_component = ecs_id(cortecs_span_t),
//         .order_by = cortecs_parser_span_compare,
//     };
//     ecs_query_t *query = ecs_query_init(cortecs_world, &desc);

//     ecs_iter_t it = ecs_query_iter(cortecs_world, query);
//     while (ecs_query_next(&it)) {
//         cortecs_span_t const *span = ecs_field(&it, cortecs_span_t, 1);
//         cortecs_parser_context_t *parent_context = ecs_query_get_group_ctx(query, it.group_id);
//         for (int i = 0; i < it.count; i++) {
//             ecs_entity_t entity = it.entities[i];
//             cortecs_span_t absolute_offset = cortecs_span_add(parent_context->absolute_offset, span[i]);
//             cortecs_parser_context_t *entity_context = ecs_query_get_group_ctx(query, entity);
//             if (entity_context) {
//                 entity_context->absolute_offset = absolute_offset;
//             }
//         }
//     }

//     ecs_query_fini(query);
//     return 0;
// }