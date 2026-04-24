/*
 * ============================================================================
 * Файл: prediction_predict_full.c
 * Требование: ПРГ:Тс-1 - Назначение и общая логика выполнения модуля
 * Описание: Полный цикл прогнозирования (все 6 действий)
 * ============================================================================
 */

#include "../include/prediction.h"
#include <stdlib.h>
#include <string.h>

/* Внешние объявления вспомогательных функций */
extern int32_t matrix_multiply(const Matrix *A, const Matrix *B, Matrix *C);
extern int32_t matrix_add(const Matrix *A, const Matrix *B, Matrix *C);
extern int32_t matrix_transpose(const Matrix *A, Matrix *AT);
extern int32_t matrix_vector_multiply(const Matrix *A, const Vector *X, Vector *Y);
extern int32_t matrix_copy(const Matrix *src, Matrix *dst);
extern int32_t vector_copy(const Vector *src, Vector *dst);

/* ============================================================================
 * Вспомогательная функция: сохранение состояния для отката
 * ============================================================================ */
static int32_t save_full_state(PredictionParams *params,
                                Vector *X_est_bak, Matrix *P_est_bak,
                                Vector *X_pred_bak, Matrix *P_pred_bak)
{
    X_est_bak->size = params->X_est->size;
    X_est_bak->data = (double*)malloc(X_est_bak->size * sizeof(double));
    if (X_est_bak->data == NULL) return PRED_ERR_MEM;
    X_est_bak->is_allocated = true;
    
    P_est_bak->rows = params->P_est->rows;
    P_est_bak->cols = params->P_est->cols;
    P_est_bak->data = (double*)malloc(P_est_bak->rows * P_est_bak->cols * sizeof(double));
    if (P_est_bak->data == NULL) { free(X_est_bak->data); return PRED_ERR_MEM; }
    P_est_bak->is_allocated = true;
    
    X_pred_bak->size = params->X_pred->size;
    X_pred_bak->data = (double*)malloc(X_pred_bak->size * sizeof(double));
    if (X_pred_bak->data == NULL) { free(X_est_bak->data); free(P_est_bak->data); return PRED_ERR_MEM; }
    X_pred_bak->is_allocated = true;
    
    P_pred_bak->rows = params->P_pred->rows;
    P_pred_bak->cols = params->P_pred->cols;
    P_pred_bak->data = (double*)malloc(P_pred_bak->rows * P_pred_bak->cols * sizeof(double));
    if (P_pred_bak->data == NULL) {
        free(X_est_bak->data); free(P_est_bak->data); free(X_pred_bak->data);
        return PRED_ERR_MEM;
    }
    P_pred_bak->is_allocated = true;
    
    vector_copy(params->X_est, X_est_bak);
    matrix_copy(params->P_est, P_est_bak);
    vector_copy(params->X_pred, X_pred_bak);
    matrix_copy(params->P_pred, P_pred_bak);
    
    return PRED_OK;
}

/* ============================================================================
 * Вспомогательная функция: восстановление состояния из резервной копии
 * ============================================================================ */
static void restore_full
