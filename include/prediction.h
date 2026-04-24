#ifndef PREDICTION_H
#define PREDICTION_H

/*
 * ============================================================================
 * Модуль: Прогнозирование (Prediction Module)
 * Стандарт: КТ-178 (DO-178), уровень критичности A
 * ============================================================================
 * 
 * Требование: ПРГ:Ту-1 - Общий состав входных данных
 * Требование: ПРГ:Тр-1 - Общий результат выполнения модуля
 * Требование: ПРГ:Тс-1 - Назначение и общая логика выполнения модуля
 * ============================================================================
 */

#include <stdint.h>
#include <stdbool.h>

/* ==================== Коды ошибок ==================== */
/* Требование: ПРГ:Ту-1.4 - Обработка ошибочных ситуаций */
#define PRED_OK         0       /* Успешное выполнение */
#define PRED_ERR_NULL   -1      /* Ошибка: нулевой указатель */
#define PRED_ERR_SIZE   -2      /* Ошибка: недопустимая размерность (<=0) */
#define PRED_ERR_MEM    -3      /* Ошибка: выделение памяти */
#define PRED_ERR_ALLOC  -4      /* Ошибка: память уже выделена */
#define PRED_ERR_INVALID -5     /* Ошибка: недопустимые параметры */
#define PRED_ERR_MATH   -6      /* Ошибка: математические вычисления */

/* ==================== Флаги состояния ==================== */
/* Требование: ПРГ:Тр-1 - Признаки результатов выполнения модуля */
typedef struct {
    bool X_updated;             /* Признак "Обновлен прогноз вектора X" */
    bool P_updated;             /* Признак "Обновлен прогноз матрицы P" */
} PredictionFlags;

/* ==================== Структуры данных ==================== */
/* Требование: ПРГ:Ту-1.1 - Структуры для хранения векторов и матриц */

typedef struct {
    uint32_t size;              /* Размерность вектора */
    double *data;               /* Данные вектора */
    bool is_allocated;          /* Флаг выделения памяти */
} Vector;

typedef struct {
    uint32_t rows;              /* Количество строк */
    uint32_t cols;              /* Количество столбцов */
    double *data;               /* Данные матрицы */
    bool is_allocated;          /* Флаг выделения памяти */
} Matrix;

/* Требование: ПРГ:Ту-1 - Структура всех входных и выходных параметров */
typedef struct {
    uint32_t n;                 /* Размерность вектора состояния */
    uint32_t p;                 /* Размерность вектора входных шумов */
    Vector *X_est;              /* Оценка вектора состояния X' */
    Matrix *P_est;              /* Оценка матрицы ковариации P' */
    Vector *X_pred;             /* Прогнозируемый вектор состояния X */
    Matrix *P_pred;             /* Прогнозируемая матрица ковариации P */
    Matrix *F;                  /* Переходная матрица F (n x n) */
    Matrix *G;                  /* Переходная матрица G (n x p) */
    Matrix *Q;                  /* Ковариационная матрица Q (p x p) */
    PredictionFlags *flags;     /* Флаги состояния */
} PredictionParams;

/* ==================== Функции инициализации ==================== */
/* Требование: ПРГ:Ту-1.1, ПРГ:Ту-1.2, ПРГ:Ту-1.3 */

int32_t pred_init_vectors(PredictionParams *params);
int32_t pred_init_matrices(PredictionParams *params);
int32_t pred_cleanup(PredictionParams *params);

/* ==================== Функции валидации ==================== */
/* Требование: ПРГ:Ту-1.4 */

bool pred_is_valid_pointers(const PredictionParams *params);
bool pred_is_valid_dimensions(const PredictionParams *params);
int32_t pred_validate_all(const PredictionParams *params);

/* ==================== Основные функции прогнозирования ==================== */

/* Требование: ПРГ:Тр-1 - Упрощённый прогноз (только обновление X_pred, P_pred) */
int32_t pred_predict(PredictionParams *params);

/* Требование: ПРГ:Тс-1 - Полная логика прогнозирования (все 6 действий) */
int32_t pred_predict_full(PredictionParams *params);

/* ==================== Функции работы с флагами ==================== */
/* Требование: ПРГ:Тр-1.5, ПРГ:Тс-1.6, ПРГ:Тс-1.7 */

PredictionFlags pred_get_flags(const PredictionParams *params);
void pred_reset_flags(PredictionFlags *flags);

#endif /* PREDICTION_H */
