#include <flecs.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unity.h>

typedef struct {
    uint32_t id;
} someid;

const int num_entities = 10000;
ECS_COMPONENT_DECLARE(someid);

typedef struct {
    ecs_entity_t *entities;
    bool **descendancies;
} test_ancestor_traversal_gold_t;

test_ancestor_traversal_gold_t test_ancestor_traversal_init_gold(ecs_world_t *world) {
    test_ancestor_traversal_gold_t gold;
    // Initialize all entities
    gold.entities = malloc(num_entities * sizeof(ecs_entity_t));
    for (uint32_t i = 0; i < num_entities; i++) {
        gold.entities[i] = ecs_new_id(world);
        ecs_set(world, gold.entities[i], someid, {.id = i});
    }

    // Randomly set up the tree
    ecs_add_pair(world, gold.entities[1], EcsChildOf, gold.entities[0]);
    uint32_t *topology = calloc(num_entities, sizeof(uint32_t));
    for (uint32_t child = 2; child < num_entities; child++) {
        uint32_t parent = rand() % (child - 1);
        topology[child] = parent;
        ecs_add_pair(world, gold.entities[child], EcsChildOf, gold.entities[parent]);
    }

    // Construct descendancies matrix
    gold.descendancies = calloc(num_entities, sizeof(bool *));
    for (int i = 0; i < num_entities; i++) {
        gold.descendancies[i] = calloc(num_entities, sizeof(bool));
    }
    for (uint32_t child = 1; child < num_entities; child++) {
        uint32_t parent = topology[child];
        while (true) {
            gold.descendancies[parent][child] = true;
            if (parent == 0) {
                break;
            }
            parent = topology[parent];
        }
    }

    return gold;
}

static void test_ancestor_traversal() {
    // Tests iteration of all (entity, descendant) pairs in a large tree.

    ecs_world_t *world = ecs_init();
    ECS_COMPONENT_DEFINE(world, someid);
    test_ancestor_traversal_gold_t gold = test_ancestor_traversal_init_gold(world);

    ecs_filter_desc_t rule_desc = (ecs_filter_desc_t){
        .terms[0] = (ecs_term_t){
            .id = ecs_id(someid),
        },
        .terms[1] = (ecs_term_t){
            .id = ecs_id(someid),
            .src = (ecs_term_id_t){
                .name = "$Descendant",
            },
        },
        .terms[2] = (ecs_term_t){
            .src = (ecs_term_id_t){
                .name = "$Descendant",
                .trav = EcsChildOf,
            },
            .first.id = EcsChildOf,
            .second.id = EcsThis,
        },
    };
    ecs_rule_t *rule = ecs_rule_init(world, &rule_desc);

    // Setup test output
    bool **out_descendancies = calloc(num_entities, sizeof(bool *));
    for (int i = 0; i < num_entities; i++) {
        out_descendancies[i] = calloc(num_entities, sizeof(bool));
    }

    // Iterate through all (entity, dencendant) pairs
    ecs_iter_t iterator = ecs_rule_iter(world, rule);
    while (ecs_rule_next(&iterator)) {
        const someid *this_id = ecs_field(&iterator, someid, 1);
        const someid *descendant_id = ecs_field(&iterator, someid, 2);
        for (int i = 0; i < iterator.count; i++) {
            // Make sure we havent iterated this pair before
            TEST_ASSERT_FALSE(out_descendancies[this_id[i].id][descendant_id[i].id]);

            // Mark as iterated
            out_descendancies[this_id[i].id][descendant_id[i].id] = true;
        }
    }

    // Compare gold and out descendancies matricies
    for (int i = 0; i < num_entities; i++) {
        for (int j = 0; j < num_entities; j++) {
            TEST_ASSERT_EQUAL(gold.descendancies[i][j], out_descendancies[i][j]);
        }
    }

    // Test cleanup
    for (int i = 0; i < num_entities; i++) {
        free(gold.descendancies[i]);
        free(out_descendancies[i]);
    }
    free(gold.entities);
    free(gold.descendancies);
    free(out_descendancies);
    ecs_rule_fini(rule);
    ecs_fini(world);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_ancestor_traversal);
    return UNITY_END();
}

void setUp() {
    // required for unity
    srand(time(NULL));
}

void tearDown() {
    // required for unity
}