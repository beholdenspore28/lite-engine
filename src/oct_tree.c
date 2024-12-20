#include "oct_tree.h"

DEFINE_LIST(oct_tree_entry_t)

oct_tree_t *oct_tree_alloc(void) {
	oct_tree_t *tree = malloc(sizeof(oct_tree_t));
	tree->parent         = NULL;
	tree->frontNorthEast = NULL;
	tree->frontNorthWest = NULL;
	tree->frontSouthEast = NULL;
	tree->frontSouthWest = NULL;
	tree->backNorthEast  = NULL;
	tree->backNorthWest  = NULL;
	tree->backSouthEast  = NULL;
	tree->backSouthWest  = NULL;
	tree->position.x     = 0;
	tree->position.y     = 0;
	tree->position.z     = 0;
	tree->octSize        = 1000;
	tree->minimumSize    = 1;
	tree->capacity       = 100;
	tree->depth          = 0;
	tree->entries        = list_oct_tree_entry_t_alloc();
	tree->isSubdivided   = false;
	return tree;
}

void oct_tree_free(oct_tree_t *tree) {
	if (tree->isSubdivided == true) {
		oct_tree_free(tree->frontNorthEast);
		oct_tree_free(tree->frontNorthWest);
		oct_tree_free(tree->frontSouthEast);
		oct_tree_free(tree->frontSouthWest);
		oct_tree_free(tree->backNorthEast);
		oct_tree_free(tree->backNorthWest);
		oct_tree_free(tree->backSouthEast);
		oct_tree_free(tree->backSouthWest);
	}
	list_oct_tree_entry_t_free(&tree->entries);
	free(tree);
}

void oct_tree_subdivide(oct_tree_t *tree) {
	tree->isSubdivided = true;
	vector3_t tPos = tree->position;
	float quarterOctSize = tree->octSize * 0.25;

	tree->frontNorthEast = oct_tree_alloc();
	tree->frontNorthEast->parent = tree;
	tree->frontNorthEast->position.x = tPos.x + quarterOctSize;
	tree->frontNorthEast->position.y = tPos.y + quarterOctSize;
	tree->frontNorthEast->position.z = tPos.z + quarterOctSize;
	tree->frontNorthEast->octSize = tree->octSize * 0.5;
	tree->frontNorthEast->minimumSize = tree->minimumSize;
	tree->frontNorthEast->depth = tree->depth + 1;

	tree->frontNorthWest = oct_tree_alloc();
	tree->frontNorthWest->parent = tree;
	tree->frontNorthWest->position.x = tPos.x - quarterOctSize;
	tree->frontNorthWest->position.y = tPos.y + quarterOctSize;
	tree->frontNorthWest->position.z = tPos.z + quarterOctSize;
	tree->frontNorthWest->octSize = tree->octSize * 0.5;
	tree->frontNorthWest->minimumSize = tree->minimumSize;
	tree->frontNorthWest->depth = tree->depth + 1;

	tree->frontSouthEast = oct_tree_alloc();
	tree->frontSouthEast->parent = tree;
	tree->frontSouthEast->position.x = tPos.x + quarterOctSize;
	tree->frontSouthEast->position.y = tPos.y - quarterOctSize;
	tree->frontSouthEast->position.z = tPos.z + quarterOctSize;
	tree->frontSouthEast->octSize = tree->octSize * 0.5;
	tree->frontSouthEast->minimumSize = tree->minimumSize;
	tree->frontSouthEast->depth = tree->depth + 1;

	tree->frontSouthWest = oct_tree_alloc();
	tree->frontSouthWest->parent = tree;
	tree->frontSouthWest->position.x = tPos.x - quarterOctSize;
	tree->frontSouthWest->position.y = tPos.y - quarterOctSize;
	tree->frontSouthWest->position.z = tPos.z + quarterOctSize;
	tree->frontSouthWest->octSize = tree->octSize * 0.5;
	tree->frontSouthWest->minimumSize = tree->minimumSize;
	tree->frontSouthWest->depth = tree->depth + 1;

	tree->backNorthEast = oct_tree_alloc();
	tree->backNorthEast->parent = tree;
	tree->backNorthEast->position.x = tPos.x + quarterOctSize;
	tree->backNorthEast->position.y = tPos.y + quarterOctSize;
	tree->backNorthEast->position.z = tPos.z - quarterOctSize;
	tree->backNorthEast->octSize = tree->octSize * 0.5;
	tree->backNorthEast->minimumSize = tree->minimumSize;
	tree->backNorthEast->depth = tree->depth + 1;

	tree->backNorthWest = oct_tree_alloc();
	tree->backNorthWest->parent = tree;
	tree->backNorthWest->position.x = tPos.x - quarterOctSize;
	tree->backNorthWest->position.y = tPos.y + quarterOctSize;
	tree->backNorthWest->position.z = tPos.z - quarterOctSize;
	tree->backNorthWest->octSize = tree->octSize * 0.5;
	tree->backNorthWest->minimumSize = tree->minimumSize;
	tree->backNorthWest->depth = tree->depth + 1;

	tree->backSouthEast = oct_tree_alloc();
	tree->backSouthEast->parent = tree;
	tree->backSouthEast->position.x = tPos.x + quarterOctSize;
	tree->backSouthEast->position.y = tPos.y - quarterOctSize;
	tree->backSouthEast->position.z = tPos.z - quarterOctSize;
	tree->backSouthEast->octSize = tree->octSize * 0.5;
	tree->backSouthEast->minimumSize = tree->minimumSize;
	tree->backSouthEast->depth = tree->depth + 1;

	tree->backSouthWest = oct_tree_alloc();
	tree->backSouthWest->parent = tree;
	tree->backSouthWest->position.x = tPos.x - quarterOctSize;
	tree->backSouthWest->position.y = tPos.y - quarterOctSize;
	tree->backSouthWest->position.z = tPos.z - quarterOctSize;
	tree->backSouthWest->octSize = tree->octSize * 0.5;
	tree->backSouthWest->minimumSize = tree->minimumSize;
	tree->backSouthWest->depth = tree->depth + 1;
}

bool oct_tree_contains(oct_tree_t *tree, vector3_t point) {
	float halfOctSize = tree->octSize * 0.5;
	return 
		point.x <= tree->position.x + halfOctSize &&
		point.x >= tree->position.x - halfOctSize &&
		point.y <= tree->position.y + halfOctSize &&
		point.y >= tree->position.y - halfOctSize &&
		point.z <= tree->position.z + halfOctSize &&
		point.z >= tree->position.z - halfOctSize;
}

bool oct_tree_insert(oct_tree_t *tree, oct_tree_entry_t entry) {
	if (oct_tree_contains(tree, entry.position) == false)
		return false;
	if (tree->entries.length < tree->capacity ||
			tree->octSize < tree->minimumSize) {
		list_oct_tree_entry_t_add(&tree->entries, entry);
		return true;
	} else {
		if (tree->isSubdivided == false)
			oct_tree_subdivide(tree);
		if (oct_tree_insert(tree->frontNorthEast, entry))
			return true;
		if (oct_tree_insert(tree->frontNorthWest, entry))
			return true;
		if (oct_tree_insert(tree->frontSouthEast, entry))
			return true;
		if (oct_tree_insert(tree->frontSouthWest, entry))
			return true;
		if (oct_tree_insert(tree->backNorthEast, entry))
			return true;
		if (oct_tree_insert(tree->backNorthWest, entry))
			return true;
		if (oct_tree_insert(tree->backSouthEast, entry))
			return true;
		if (oct_tree_insert(tree->backSouthWest, entry))
			return true;
	}
	return false;
}
