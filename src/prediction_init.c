/*
 * ============================================================================
 * Файл: prediction_init.c
 * Требования: ПРГ:Ту-1.1 - Выделение памяти для векторов и матриц
 *             ПРГ:Ту-1.2 - Инициализация размерностей
 *             ПРГ:Ту-1.3 - Деинициализация (освобождение памяти)
 * ============================================================================
 */

#include "../include/prediction.h"
#include <stdlib.h>
#include <string.h>

/* ============================================================================
 * ПРГ:Ту-1.1, ПРГ:Ту-1.2 - Выделение памяти для векторов
 * Постусловие: память выделена, data != NULL, size = n, is_allocated = true
 * ============================================================================ */
int32_t pred_init_vectors(PredictionParams *params)
{
    if (params == NULL || params->X_est == NULL || params->X_pred == NULL) {
        return PRED_ERR_NULL;
    }
    
    if (params->n == 0) {
        return PRED_ERR_SIZE;
    }
    
    /* Выделение для X_est */
    if (!params->X_est->is_allocated) {
        params->X_est->data = (double*)malloc(params->n * sizeof(double));
        if (params->X_est->data == NULL) {
            return PRED_ERR_MEM;
        }
        params->X_est->is_allocated = true;
        params->X_est->size = params->n;
        memset(params->X_est->data, 0, params->n * sizeof(double));
    } else {
        return PRED_ERR_ALLOC;
    }
    
    /* Выделение для X_pred */
    if (!params->X_pred->is_allocated) {
        params->X_pred->data = (double*)malloc(params->n * sizeof(double));
        if (params->X_pred->data == NULL) {
            free(params->X_est->data);
            params->X_est->data = NULL;
            params->X_est->is_allocated = false;
            return PRED_ERR_MEM;
        }
        params->X_pred->is_allocated = true;
        params->X_pred->size = params->n;
        memset(params->X_pred->data, 0, params->n * sizeof(double));
    } else {
        free(params->X_est->data);
        params->X_est->data = NULL;
        params->X_est->is_allocated = false;
        return PRED_ERR_ALLOC;
    }
    
    return PRED_OK;
}

/* ============================================================================
 * ПРГ:Ту-1.1, ПРГ:Ту-1.2 - Выделение памяти для матриц
 * Постусловие: память выделена, размерности установлены, is_allocated = true
 * ============================================================================ */
int32_t pred_init_matrices(PredictionParams *params)
{
    uint32_t f_size, g_size, q_size, p_est_size, p_pred_size;
    
    if (params == NULL || params->P_est == NULL || params->P_pred == NULL ||
        params->F == NULL || params->G == NULL || params->Q == NULL) {
        return PRED_ERR_NULL;
    }
    
    if (params->n == 0 || params->p == 0) {
        return PRED_ERR_SIZE;
    }
    
    f_size = params->n * params->n;
    g_size = params->n * params->p;
    q_size = params->p * params->p;
    p_est_size = params->n * params->n;
    p_pred_size = params->n * params->n;
    
    /* P_est (n x n) */
    if (!params->P_est->is_allocated) {
        params->P_est->data = (double*)malloc(p_est_size * sizeof(double));
        if (params->P_est->data == NULL) return PRED_ERR_MEM;
        params->P_est->is_allocated = true;
        params->P_est->rows = params->n;
        params->P_est->cols = params->n;
        memset(params->P_est->data, 0, p_est_size * sizeof(double));
    } else {
        return PRED_ERR_ALLOC;
    }
    
    /* P_pred (n x n) */
    if (!params->P_pred->is_allocated) {
        params->P_pred->data = (double*)malloc(p_pred_size * sizeof(double));
        if (params->P_pred->data == NULL) {
            free(params->P_est->data);
            params->P_est->data = NULL;
            params->P_est->is_allocated = false;
            return PRED_ERR_MEM;
        }
        params->P_pred->is_allocated = true;
        params->P_pred->rows = params->n;
        params->P_pred->cols = params->n;
        memset(params->P_pred->data, 0, p_pred_size * sizeof(double));
    } else {
        free(params->P_est->data);
        params->P_est->data = NULL;
        params->P_est->is_allocated = false;
        return PRED_ERR_ALLOC;
    }
    
    /* F (n x n) */
    if (!params->F->is_allocated) {
        params->F->data = (double*)malloc(f_size * sizeof(double));
        if (params->F->data == NULL) {
            pred_cleanup(params);
            return PRED_ERR_MEM;
        }
        params->F->is_allocated = true;
        params->F->rows = params->n;
        params->F->cols = params->n;
        memset(params->F->data, 0, f_size * sizeof(double));
    } else {
        return PRED_ERR_ALLOC;
    }
    
    /* G (n x p) */
    if (!params->G->is_allocated) {
        params->G->data = (double*)malloc(g_size * sizeof(double));
        if (params->G->data == NULL) {
            pred_cleanup(params);
            return PRED_ERR_MEM;
        }
        params->G->is_allocated = true;
        params->G->rows = params->n;
        params->G->cols = params->p;
        memset(params->G->data, 0, g_size * sizeof(double));
    } else {
        return PRED_ERR_ALLOC;
    }
    
    /* Q (p x p) */
    if (!params->Q->is_allocated) {
        params->Q->data = (double*)malloc(q_size * sizeof(double));
        if (params->Q->data == NULL) {
            pred_cleanup(params);
            return PRED_ERR_MEM;
        }
        params->Q->is_allocated = true;
        params->Q->rows = params->p;
        params->Q->cols = params->p;
        memset(params->Q->data, 0, q_size * sizeof(double));
    } else {
        return PRED_ERR_ALLOC;
    }
    
    /* Инициализация флагов */
    if (params->flags != NULL) {
        params->flags->X_updated = false;
        params->flags->P_updated = false;
    }
    
    return PRED_OK;
}

/* ============================================================================
 * ПРГ:Ту-1.3 - Освобождение всей выделенной памяти
 * Постусловие: вся память освобождена, is_allocated = false
 * ============================================================================ */
int32_t pred_cleanup(PredictionParams *params)
{
    if (params == NULL) {
        return PRED_ERR_NULL;
    }
    
    /* Векторы */
    if (params->X_est != NULL && params->X_est->is_allocated) {
        free(params->X_est->data);
        params->X_est->data = NULL;
        params->X_est->is_allocated = false;
        params->X_est->size = 0;
    }
    
    if (params->X_pred != NULL && params->X_pred->is_allocated) {
        free(params->X_pred->data);
        params->X_pred->data = NULL;
        params->X_pred->is_allocated = false;
        params->X_pred->size = 0;
    }
    
    /* Матрицы */
    if (params->P_est != NULL && params->P_est->is_allocated) {
        free(params->P_est->data);
        params->P_est->data = NULL;
        params->P_est->is_allocated = false;
        params->P_est->rows = 0;
        params->P_est->cols = 0;
    }
    
    if (params->P_pred != NULL && params->P_pred->is_allocated) {
        free(params->P_pred->data);
        params->P_pred->data = NULL;
        params->P_pred->is_allocated = false;
        params->P_pred->rows = 0;
        params->P_pred->cols = 0;
    }
    
    if (params->F != NULL && params->F->is_allocated) {
        free(params->F->data);
        params->F->data = NULL;
        params->F->is_allocated = false;
        params->F->rows = 0;
        params->F->cols = 0;
    }
    
    if (params->G != NULL && params->G->is_allocated) {
        free(params->G->data);
        params->G->data = NULL;
        params->G->is_allocated = false;
        params->G->rows = 0;
        params->G->cols = 0;
    }
    
    if (params->Q != NULL && params->Q->is_allocated) {
        free(params->Q->data);
        params->Q->data = NULL;
        params->Q->is_allocated = false;
        params->Q->rows = 0;
        params->Q->cols = 0;
    }
    
    /* Сброс флагов */
    if (params->flags != NULL) {
        params->flags->X_updated = false;
        params->flags->P_updated = false;
    }
    
    return PRED_OK;
}
