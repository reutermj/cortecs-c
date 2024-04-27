#include <parser.h>

// typedef struct {
//     int32_t abs_pos;
// } group_ctx;

// static void *on_group_create(ecs_world_t *world, uint64_t group_id, void *group_by_arg) {
//     (void)group_by_arg;
//     (void)group_id;
//     group_ctx *ctx = calloc(1, sizeof(group_ctx));
//     return ctx;
// }

// static void on_group_delete(ecs_world_t *world, uint64_t group_id, void *ctx, void *group_by_arg) {
//     (void)group_by_arg;
//     (void)group_id;
//     free(ctx);
// }

// int compare_span(ecs_entity_t e1, const void *v1, ecs_entity_t e2, const void *v2) {
//     const cortecs_span_t *span1 = v1;
//     const cortecs_span_t *span2 = v2;
//     return 0;  // todo create method to compare spans
// }

// void bla() {
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
//         .on_group_create = on_group_create,
//         .on_group_delete = on_group_delete,
//         .order_by_component = ecs_id(cortecs_span_t),
//         .order_by = compare_span,
//     };
//     ecs_query_t *query = ecs_query_init(world, &desc);

//     ecs_iter_t it = ecs_query_iter(world, query);
//     while (ecs_query_next(&it)) {
//         cortecs_span_t const *span = ecs_field(&it, cortecs_span_t, 1);
//         group_ctx *ctx = ecs_query_get_group_ctx(query, it.group_id);
//         for (int i = 0; i < it.count; i++) {
//             ecs_entity_t entity = it.entities[i];
//             uint32_t abs_pos = ctx->abs_pos + span[i].pos;
//             group_ctx *entity_ctx = ecs_query_get_group_ctx(query, entity);
//             if (entity_ctx) {
//                 entity_ctx->abs_pos = abs_pos;
//             }
//         }
//     }

//     ecs_query_fini(query);
// }