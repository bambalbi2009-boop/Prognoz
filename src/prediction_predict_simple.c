/*
 * ============================================================================
 * Файл: prediction_predict_simple.c
 * Требование: ПРГ:Тр-1 - Общий результат выполнения модуля
 * Описание: Упрощённый прогноз (только обновление X_pred и P_pred)
 * ============================================================================
 */

#include "../include/prediction.h"

/* Внешние объявления вспомогательных функций */
extern int32_t matrix_multiply(const Matrix *A, const Matrix *B, Matrix *C);
extern int32_t matrix_add(const Matrix *A, const Matrix *B, Matrix *C);
extern int32_t matrix_transpose(const Matrix *A, Matrix *AT);
extern int32_t matrix_vector_multiply(const Matrix *A, const Vector *X, Vector *Y);
extern int32_t matrix_copy(const Matrix *src, Matrix *dst);

/* ============================================================================
 * ПРГ:Тр-1 - Упрощённый прогноз (только X_pred и P_pred, без обновления X_est, P_est)
 * Постусловие: X_pred = F * X_est, P_pred = F * P_est * F^T + G * Q * G^T
 *              flags->X_updated = true, flags->P_updated = true при успехе
 * ============================================================================ */
int32_t pred_predict(PredictionParams *params)
{
    int32_t ret;
    Matrix temp1, temp2, F_transpose, temp_Q;
    
    if (params == NULL) {
        return PRED_ERR_NULL;
    }
    
    ret = pred_validate_all(params);
    if (ret != PRED_OK) {
        return ret;
    }
    
    /* Прогноз вектора: X_pred = F * X_est */
    ret = matrix_vector_multiply(params->F, params->X_est, params->X_pred);
    if (ret != PRED_OK) {
        return ret;
    }
    
    /* Выделение временных матриц */
    temp1.rows = params->n; temp1.cols = params->n;
    temp1.data = (double*)malloc(params->n * params->n * sizeof(double));
    temp1.is_allocated = true;
    if (temp1.data == NULL) return PRED_ERR_MEM;
    
    temp2.rows = params->n; temp2.cols = params->n;
    temp2.data = (double*)malloc(params->n * params->n * sizeof(double));
    temp2.is_allocated = true;
    if (temp2.data == NULL) { free(temp1.data); return PRED_ERR_MEM; }
    
    F_transpose.rows = params->n; F_transpose.cols = params->n;
    F_transpose.data = (double*)malloc(params->n * params->n * sizeof(double));
    F_transpose.is_allocated = true;
    if (F_transpose.data == NULL) { free(temp1.data); free(temp2.data); return PRED_ERR_MEM; }
    
    temp_Q.rows = params->n; temp_Q.cols = params->n;
    temp_Q.data = (double*)malloc(params->n * params->n * sizeof(double));
    temp_Q.is_allocated = true;
    if (temp_Q.data == NULL) { free(temp1.data); free(temp2.data); free(F_transpose.data); return PRED_ERR_MEM; }
    
    /* P_pred = F * P_est * F^T */
    ret = matrix_multiply(params->F, params->P_est, &temp1);
    if (ret == PRED_OK) {
        ret = matrix_transpose(params->F, &F_transpose);
    }
    if (ret == PRED_OK) {
        ret = matrix_multiply(&temp1, &F_transpose, &temp2);
    }
    
    /* G * Q * G^T */
    if (ret == PRED_OK) {
        Matrix G_transpose;
        G_transpose.rows = params->p;
        G_transpose.cols = params->n;
        G_transpose.data = (double*)malloc(params->n * params->p * sizeof(double));
        G_transpose.is_allocated = true;
        if (G_transpose.data == NULL) {
            ret = PRED_ERR_MEM;
        } else {
            ret = matrix_multiply(params->G, params->Q, &temp1);
            if (ret == PRED_OK) {
                ret = matrix_transpose(params->G, &G_transpose);
            }
            if (ret == PRED_OK) {
                ret = matrix_multiply(&temp1, &G_transpose, &temp_Q);
            }
            free(G_transpose.data);
        }
    }
    
    /* Суммирование */
    if (ret == PRED_OK) {
        ret = matrix_add(&temp2, &temp_Q, params->P_pred);
    }
    
    /* Освобождение временной памяти */
    free(temp1.data);
    free(temp2.data);
    free(F_transpose.data);
    free(temp_Q.data);
    
    /* Установка флагов */
    if (ret == PRED_OK && params->flags != NULL) {
        params->flags->X_updated = true;
        params->flags->P_updated = true;
    } else if (params->flags != NULL) {
        params->flags->X_updated = false;
        params->flags->P_updated = false;
    }
    
    return ret;
}

/* ============================================================================
 * ПРГ:Тр-1.5 - Функции работы с флагами
 * ============================================================================ */
PredictionFlags pred_get_flags(const PredictionParams *params)
{
    PredictionFlags empty = {false, false};
    
    if (params == NULL || params->flags == NULL) {
        return empty;
    }
    
    return *(params->flags);
}

void pred_reset_flags(PredictionFlags *flags)
{
    if (flags != NULL) {
        flags->X_updated = false;
        flags->P_updated = false;
    }
}
