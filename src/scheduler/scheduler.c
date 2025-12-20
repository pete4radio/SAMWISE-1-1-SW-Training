/**
 * @author  Niklas Vainio
 * @date    2024-08-25
 *
 * This file defines the tasks and states of the state machine, used for
 * scheduling and dispatching tasks on the satellite.
 */

#include "scheduler.h"
#include "states.h"

sched_state_t *overridden_state = NULL;

/* Timestamp of last runtime-stats log */
static absolute_time_t last_stats_log;

/*
 * Include the actual state machine
 */
static const sched_state_t *all_states[] = {&init_state,
#ifdef BRINGUP
                                            &bringup_state,
#endif
                                            &running_state};
static sched_state_t *const initial_state = &init_state;
static size_t n_tasks = 0;
static sched_task_t *all_tasks[num_states * MAX_TASKS_PER_STATE];

/**
 * Initialize the state machine.
 *
 * @param slate     Pointer to the slate.
 */
void sched_init(slate_t *slate)
{
    /*
     * Check that each state has a valid number of tasks, and enumerate all
     * tasks.
     */
    for (size_t i = 0; i < num_states; i++)
    {
        ASSERT(all_states[i]->num_tasks <= MAX_TASKS_PER_STATE);
        for (size_t j = 0; j < all_states[i]->num_tasks; j++)
        {
            bool is_duplicate = 0;
            for (size_t k = 0; k < n_tasks; k++)
            {
                if (all_tasks[k] == all_states[i]->task_list[j])
                    is_duplicate = 1;
            }
            if (!is_duplicate)
            {
                all_tasks[n_tasks] = all_states[i]->task_list[j];
                n_tasks++;
            }
        }
    }

    LOG_DEBUG("sched: Enumerated %d tasks", n_tasks);

    /*
     * Initialize all tasks.
     */
    for (size_t i = 0; i < n_tasks; i++)
    {
        LOG_DEBUG("sched: Initializing task %s", all_tasks[i]->name);
        all_tasks[i]->task_init(slate);
        /* initialize runtime tracking buffer */
        for (size_t r = 0; r < 5; ++r)
            all_tasks[i]->runtime[r] = 0;
        all_tasks[i]->runtime_next = 0;
    }

    for (size_t i = 0; i < n_tasks; i++)
    {
        all_tasks[i]->next_dispatch =
            make_timeout_time_ms(all_tasks[i]->dispatch_period_ms);
    }

    /*
     * Enter the init state by default
     */
    slate->current_state = initial_state;
    slate->entered_current_state_time = get_absolute_time();
    slate->time_in_current_state_ms = 0;

    /* Initialize stats log timer */
    last_stats_log = get_absolute_time();

    LOG_DEBUG("sched: Done initializing!");
}

/**
 * Dispatch the state machine. Runs any of the current state's tasks which are
 * due, and transitions into the next state.
 *
 * @param slate     Pointer to the slate.
 */
void sched_dispatch(slate_t *slate)
{
    sched_state_t *current_state_info = slate->current_state;

    /*
     * Loop through all of this state's tasks
     */

    //. Print the average run time once every 60 seconds
    absolute_time_t now = get_absolute_time();
    static absolute_time_t last_runtime_print = {0};

    {
    for (size_t i = 0; i < current_state_info->num_tasks; i++)
    {
        sched_task_t *task = current_state_info->task_list[i];

        /*
         * Check if this task is due and if so, dispatch it
         */
        if (absolute_time_diff_us(task->next_dispatch, get_absolute_time()) > 0)
        {
            task->next_dispatch =
                make_timeout_time_ms(task->dispatch_period_ms);

            /* measure dispatch runtime */
            absolute_time_t t_start = get_absolute_time();
            task->task_dispatch(slate);     // Run the task
            absolute_time_t t_end = get_absolute_time();

            int64_t diff_us = absolute_time_diff_us(t_start, t_end);
            if (diff_us < 0)
                diff_us = 0;
            uint32_t diff_ms = (uint32_t)(diff_us / 1000);

            /* clamp to 255 for uint8_t storage */
            uint8_t store_ms = diff_ms > 255 ? 255 : (uint8_t)diff_ms;

            task->runtime[task->runtime_next] = store_ms;
            task->runtime_next = (task->runtime_next + 1) % 5;
        }
    }

    slate->time_in_current_state_ms =
        absolute_time_diff_us(slate->entered_current_state_time,
                              get_absolute_time()) /
        1000;

    /* Log all task averages every 60 seconds */
    if (absolute_time_diff_us(last_stats_log, get_absolute_time()) >=
        60LL * 1000LL * 1000LL)
    {
        for (size_t ti = 0; ti < n_tasks; ++ti)
        {
            sched_task_t *t = all_tasks[ti];
            uint32_t sum = 0;
            uint8_t count = 0;
            for (size_t r = 0; r < 5; ++r)
            {
                if (t->runtime[r] != 0)
                {
                    sum += t->runtime[r];
                    count++;
                }
            }
            uint32_t avg = count ? (sum / count) : 0;
            LOG_DEBUG("sched: stats %s avg %u ms", t->name, avg);
        }
        last_stats_log = get_absolute_time();
    }

    /*
     * Transition to the next state, if required.
     */
    sched_state_t *next_state;
    if (overridden_state)
    {
        next_state = overridden_state;
        overridden_state = NULL;
    }
    else
    {
        next_state = current_state_info->get_next_state(slate);
    }

    if (next_state != current_state_info)
    {
        LOG_DEBUG("sched: Transitioning to state %s", next_state->name);

        slate->current_state = next_state;
        slate->entered_current_state_time = get_absolute_time();
        slate->time_in_current_state_ms = 0;
    }
}
