/*
 * ============================================================================
 * Файл: prediction_utils.c
 * Описание: Общие вспомогательные математические функции для всех требований
 * ============================================================================
 */

#include "../include/prediction.h"
#include <string.h>

/* ============================================================================
 * Вспомогательная функция: умножение матриц C = A * B
 * ============================================================================ */
int32_t matrix_multiply(const Matrix *A, const Matrix *B, Matrix *C)
{
    uint32_t i, j, k;
    double sum;
    
    if (A == NULL || B == NULL || C == NULL) {
        return PRED_ERR_NULL;
    }
    
    if (A->cols != B->rows || C->rows != A->rows || C->cols != B->cols) {
        return PRED_ERR_INVALID;
    }
    
    if (!A->is_allocated || !B->is_allocated || !C->is_allocated) {
        return PRED_ERR_MEM;
    }
    
    for (i = 0; i < C->rows; i++) {
        for (j = 0; j < C->cols; j++) {
            sum = 0.0;
            for (k = 0; k < A->cols; k++) {
                sum += A->data[i * A->cols + k] * B->data[k * B->cols + j];
            }
            C->data[i * C->cols + j] = sum;
        }
    }
    
    return PRED_OK;
}

/* ============================================================================
 * Вспомогательная функция: сложение матриц C = A + B
 * ============================================================================ */
int32_t matrix_add(const Matrix *A, const Matrix *B, Matrix *C)
{
    uint32_t i;
    uint32_t size;
    
    if (A == NULL || B == NULL || C == NULL) {
        return PRED_ERR_NULL;
    }
    
    if (A->rows != B->rows || A->rows != C->rows ||
        A->cols != B->cols || A->cols != C->cols) {
        return PRED_ERR_INVALID;
    }
    
    if (!A->is_allocated || !B->is_allocated || !C->is_allocated) {
        return PRED_ERR_MEM;
    }
    
    size = A->rows * A->cols;
    for (i = 0; i < size; i++) {
        C->data[i] = A->data[i] + B->data[i];
    }
    
    return PRED_OK;
}

/* ============================================================================
 * Вспомогательная функция: транспонирование матрицы AT = A^T
 * ============================================================================ */
int32_t matrix_transpose(const Matrix *A, Matrix *AT)
{
    uint32_t i, j;
    
    if (A == NULL || AT == NULL) {
        return PRED_ERR_NULL;
    }
    
    if (AT->rows != A->cols || AT->cols != A->rows) {
        return PRED_ERR_INVALID;
    }
    
    if (!A->is_allocated || !AT->is_allocated) {
        return PRED_ERR_MEM;
    }
    
    for (i = 0; i < A->rows; i++) {
        for (j = 0; j < A->cols; j++) {
            AT->data[j * AT->cols + i] = A->data[i * A->cols + j];
        }
    }
    
    return PRED_OK;
}

/* ============================================================================
 * Вспомогательная функция: умножение матрицы на вектор Y = A * X
 * ============================================================================ */
int32_t matrix_vector_multiply(const Matrix *A, const Vector *X, Vector *Y)
{
    uint32_t i, j;
    double sum;
    
    if (A == NULL || X == NULL || Y == NULL) {
        return PRED_ERR_NULL;
    }
    
    if (A->cols != X->size || Y->size != A->rows) {
        return PRED_ERR_INVALID;
    }
    
    if (!A->is_allocated || !X->is_allocated || !Y->is_allocated) {
        return PRED_ERR_MEM;
    }
    
    for (i = 0; i < A->rows; i++) {
        sum = 0.0;
        for (j = 0; j < A->cols; j++) {
            sum += A->data[i * A->cols + j] * X->data[j];
        }
        Y->data[i] = sum;
    }
    
    return PRED_OK;
}

/* ============================================================================
 * Вспомогательная функция: копирование вектора dst = src
 * ============================================================================ */
int32_t vector_copy(const Vector *src, Vector *dst)
{
    if (src == NULL || dst == NULL) {
        return PRED_ERR_NULL;
    }
    
    if (src->size != dst->size) {
        return PRED_ERR_INVALID;
    }
    
    if (!src->is_allocated || !dst->is_allocated) {
        return PRED_ERR_MEM;
    }
    
    memcpy(dst->data, src->data, src->size * sizeof(double));
    
    return PRED_OK;
}

/* ============================================================================
 * Вспомогательная функция: копирование матрицы dst = src
 * ============================================================================ */
int32_t matrix_copy(const Matrix *src, Matrix *dst)
{
    uint32_t size;
    
    if (src == NULL || dst == NULL) {
        return PRED_ERR_NULL;
    }
    
    if (src->rows != dst->rows || src->cols != dst->cols) {
        return PRED_ERR_INVALID;
    }
    
    if (!src->is_allocated || !dst->is_allocated) {
        return PRED_ERR_MEM;
    }
    
    size = src->rows * src->cols;
    memcpy(dst->data, src->data, size * sizeof(double));
    
    return PRED_OK;
}
