#pragma once

#include "blib/blib.h"
#include "blib/math3d.h"

typedef struct oct_tree {
	vector3_t position;
	float octSize;
	float minimumSize;
	uint32_t capacity;
	list_vector3_t points;
	list_uint32_t data;
	bool isSubdivided;
	int depth;
	struct oct_tree *parent;
	struct oct_tree *frontNorthEast;
	struct oct_tree *frontNorthWest;
	struct oct_tree *frontSouthEast;
	struct oct_tree *frontSouthWest;
	struct oct_tree *backNorthEast;
	struct oct_tree *backNorthWest;
	struct oct_tree *backSouthEast;
	struct oct_tree *backSouthWest;
} oct_tree_t;

oct_tree_t *oct_tree_alloc(void);
void oct_tree_free(oct_tree_t *tree);
void oct_tree_subdivide(oct_tree_t *tree);
bool oct_tree_contains(oct_tree_t *tree, vector3_t point);
bool oct_tree_insert(oct_tree_t *tree, uint32_t data, vector3_t point);
