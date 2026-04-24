/*
 * ============================================================================
 * Файл: prediction_validate.c
 * Требование: ПРГ:Ту-1.4 - Проверка корректности входных параметров
 * ============================================================================
 */

#include "../include/prediction.h"

/* ============================================================================
 * ПРГ:Ту-1.4 - Проверка корректности всех указателей
 * Постусловие: возвращает true только если все указатели валидны
 * ============================================================================ */
bool pred_is_valid_pointers(const PredictionParams *params)
{
    if (params == NULL) {
        return false;
    }
    
    if (params->X_est == NULL || params->P_est == NULL ||
        params->X_pred == NULL || params->P_pred == NULL ||
        params->F == NULL || params->G == NULL || params->Q == NULL ||
        params->flags == NULL) {
        return false;
    }
    
    return true;
}

/* ============================================================================
 * ПРГ:Ту-1.4 - Проверка корректности размерностей
 * Постусловие: возвращает true только если размерности согласованы
 * ============================================================================ */
bool pred_is_valid_dimensions(const PredictionParams *params)
{
    if (params == NULL) {
        return false;
    }
    
    if (params->n == 0 || params->p == 0) {
        return false;
    }
    
    /* Векторы */
    if (params->X_est->is_allocated && params->X_est->size != params->n) {
        return false;
    }
    
    if (params->X_pred->is_allocated && params->X_pred->size != params->n) {
        return false;
    }
    
    /* Матрицы */
    if (params->P_est->is_allocated && 
        (params->P_est->rows != params->n || params->P_est->cols != params->n)) {
        return false;
    }
    
    if (params->P_pred->is_allocated && 
        (params->P_pred->rows != params->n || params->P_pred->cols != params->n)) {
        return false;
    }
    
    if (params->F->is_allocated && 
        (params->F->rows != params->n || params->F->cols != params->n)) {
        return false;
    }
    
    if (params->G->is_allocated && 
        (params->G->rows != params->n || params->G->cols != params->p)) {
        return false;
    }
    
    if (params->Q->is_allocated && 
        (params->Q->rows != params->p || params->Q->cols != params->p)) {
        return false;
    }
    
    return true;
}

/* ============================================================================
 * ПРГ:Ту-1.4 - Комплексная проверка всех параметров
 * Постусловие: возвращает PRED_OK если все проверки пройдены
 * ============================================================================ */
int32_t pred_validate_all(const PredictionParams *params)
{
    if (!pred_is_valid_pointers(params)) {
        return PRED_ERR_NULL;
    }
    
    if (!pred_is_valid_dimensions(params)) {
        return PRED_ERR_INVALID;
    }
    
    return PRED_OK;
}
