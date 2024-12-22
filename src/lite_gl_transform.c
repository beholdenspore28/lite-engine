#include "lite_gl.h"

DEFINE_LIST(transform_t)

void transform_calculate_matrix(transform_t *t) {
	matrix4_t translation = matrix4_translate(t->position);
	matrix4_t rotation = quaternion_to_matrix4(t->rotation);
	matrix4_t scale = matrix4_scale(t->scale);
	t->matrix = matrix4_multiply(rotation, translation);
	t->matrix = matrix4_multiply(scale, t->matrix);
}

void transform_calculate_view_matrix(transform_t *t) {
	matrix4_t translation = matrix4_translate(vector3_negate(t->position));
	matrix4_t rotation = quaternion_to_matrix4(quaternion_conjugate(t->rotation));
	matrix4_t scale = matrix4_scale(t->scale);
	t->matrix = matrix4_multiply(translation, rotation);
	t->matrix = matrix4_multiply(scale, t->matrix);
}

vector3_t transform_basis_forward(transform_t t, float magnitude) {
	return vector3_rotate(vector3_forward(magnitude), t.rotation);
}

vector3_t transform_basis_up(transform_t t, float magnitude) {
	return vector3_rotate(vector3_up(magnitude), t.rotation);
}

vector3_t transform_basis_right(transform_t t, float magnitude) {
	return vector3_rotate(vector3_right(magnitude), t.rotation);
}

vector3_t transform_basis_back(transform_t t, float magnitude) {
	return vector3_rotate(vector3_back(magnitude), t.rotation);
}

vector3_t transform_basis_down(transform_t t, float magnitude) {
	return vector3_rotate(vector3_down(magnitude), t.rotation);
}

vector3_t transform_basis_left(transform_t t, float magnitude) {
	return vector3_rotate(vector3_left(magnitude), t.rotation);
}

