/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debounce_api.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/12 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/12 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEBOUNCE_API_H
# define DEBOUNCE_API_H

# include <sys/time.h>

/**
 * API Contract: Keyboard Debounce System
 * Feature: 015-bvh-viz-improvements
 * Version: 1.0.0
 *
 * Purpose:
 *   Implement 1-second keyboard debounce mechanism with timer management
 *   to prevent rendering stuttering during rapid keyboard navigation.
 *
 * Contract Guarantees:
 *   - Timer Precision: Microsecond accuracy using gettimeofday()
 *   - Delay Duration: 1,000,000 microseconds (1 second)
 *   - State Machine: IDLE → PENDING → RENDERING transitions
 *   - Input Queue: FIFO buffer for inputs during render
 *   - Fixed Allocation: Stack-based structures, no malloc/free
 *
 * 42 School Constraints:
 *   - Uses only allowed functions: gettimeofday, memcpy
 *   - No pthread, fork, or external parallelization
 *   - Single-threaded event loop architecture
 *   - Fixed-size pre-allocated structures
 */

/**
 * Constants
 */
# define TIMER_POOL_SIZE	4		/* Number of independent timers */
# define INPUT_QUEUE_SIZE	16		/* Maximum queued inputs */
# define DEBOUNCE_DELAY_US	1000000	/* 1 second in microseconds */

/**
 * Timer States
 */
typedef enum e_timer_state
{
	TIMER_IDLE,			/* No timer active, no pending render */
	TIMER_PENDING,		/* Timer counting down, waiting to expire */
	TIMER_RENDERING		/* Render in progress, queue new inputs */
}	t_timer_state;

/**
 * Timer IDs
 *
 * Each timer slot corresponds to a different input category.
 */
typedef enum e_timer_id
{
	TIMER_NAVIGATION,	/* Camera movement, arrow keys */
	TIMER_HUD,			/* HUD toggle, pagination */
	TIMER_BVH_VIS,		/* BVH tree visualization */
	TIMER_SELECTION		/* Object selection cycling */
}	t_timer_id;

/**
 * Debounce Timer Structure
 */
typedef struct s_debounce_timer
{
	long			start_usec;		/* Timer start timestamp (microseconds) */
	t_timer_state	state;			/* Current state (IDLE/PENDING/RENDERING) */
	int				accumulated;	/* Number of inputs accumulated */
}	t_debounce_timer;

/**
 * Input Event Structure
 */
typedef struct s_input_event
{
	int		keycode;			/* X11/MLX keycode value */
	long	timestamp_usec;		/* Event arrival timestamp */
}	t_input_event;

/**
 * Input Queue Structure (Circular Buffer)
 */
typedef struct s_input_queue
{
	t_input_event	events[INPUT_QUEUE_SIZE];	/* Fixed circular buffer */
	int				head;						/* Write position */
	int				tail;						/* Read position */
	int				count;						/* Current queue size */
}	t_input_queue;

/**
 * Timer Pool Structure
 */
typedef struct s_timer_pool
{
	t_debounce_timer	timers[TIMER_POOL_SIZE];	/* Fixed timer slots */
	t_input_queue		input_queue;				/* Shared input queue */
}	t_timer_pool;

/**
 * @brief Initialize timer pool
 *
 * Initializes all timer slots to IDLE state and clears input queue.
 * Must be called once at window initialization.
 *
 * @param pool Pointer to timer pool structure
 *
 * @pre pool is non-NULL
 * @post All timers in IDLE state with zero timestamps
 * @post Input queue empty (count=0, head=0, tail=0)
 *
 * Thread Safety: Not thread-safe (call once during init)
 * Complexity: O(1) time, O(1) space
 */
void	init_timer_pool(t_timer_pool *pool);

/**
 * @brief Get timer by ID
 *
 * Returns pointer to specified timer slot.
 *
 * @param pool Pointer to timer pool
 * @param id Timer identifier (TIMER_NAVIGATION, etc.)
 *
 * @return Pointer to timer structure, or NULL if invalid ID
 *
 * @pre pool is non-NULL
 * @pre id is valid enum value (0-3)
 * @post Return value is pointer into pool array (not separately allocated)
 *
 * Thread Safety: Safe (read-only access)
 * Complexity: O(1) time, O(1) space
 */
t_debounce_timer	*get_timer(t_timer_pool *pool, t_timer_id id);

/**
 * @brief Check if timer expired
 *
 * Returns true if timer is in PENDING state and 1 second has elapsed
 * since start_usec.
 *
 * @param timer Pointer to debounce timer
 *
 * @return 1 if expired, 0 otherwise
 *
 * @pre timer is non-NULL
 * @post Timer state unchanged
 *
 * Thread Safety: Safe (read-only)
 * Complexity: O(1) time, O(1) space
 */
int	is_timer_expired(t_debounce_timer *timer);

/**
 * @brief Reset timer countdown
 *
 * Updates timer start timestamp to current time and increments
 * accumulated input count. Used when new input arrives during PENDING state.
 *
 * @param timer Pointer to debounce timer
 *
 * @pre timer is non-NULL
 * @pre timer->state is PENDING
 * @post start_usec updated to current timestamp
 * @post accumulated incremented by 1
 *
 * Thread Safety: Not thread-safe (single-threaded event loop)
 * Complexity: O(1) time, O(1) space
 */
void	reset_timer(t_debounce_timer *timer);

/**
 * @brief Start rendering phase
 *
 * Transitions timer from PENDING to RENDERING state.
 * Call before executing render operation.
 *
 * @param timer Pointer to debounce timer
 *
 * @pre timer is non-NULL
 * @pre timer->state is PENDING
 * @post timer->state is TIMER_RENDERING
 *
 * Thread Safety: Not thread-safe
 * Complexity: O(1) time, O(1) space
 */
void	start_rendering(t_debounce_timer *timer);

/**
 * @brief Finish rendering phase
 *
 * Transitions timer from RENDERING to IDLE or PENDING based on
 * whether queued inputs exist.
 *
 * @param timer Pointer to debounce timer
 * @param has_queued 1 if input queue has events, 0 otherwise
 *
 * @pre timer is non-NULL
 * @pre timer->state is TIMER_RENDERING
 * @post If has_queued=0: state=IDLE, accumulated=0
 * @post If has_queued=1: state=PENDING, start_usec=current time
 *
 * Thread Safety: Not thread-safe
 * Complexity: O(1) time, O(1) space
 */
void	finish_rendering(t_debounce_timer *timer, int has_queued);

/**
 * @brief Enqueue input event
 *
 * Adds keyboard event to input queue. Used when input arrives
 * during RENDERING state.
 *
 * @param queue Pointer to input queue
 * @param keycode MLX keycode value
 *
 * @return 1 on success, 0 if queue is full
 *
 * @pre queue is non-NULL
 * @pre keycode is valid MLX key constant
 * @post If queue not full: event added, head advanced, count incremented
 * @post If queue full: no changes, returns 0
 *
 * Thread Safety: Not thread-safe
 * Complexity: O(1) time, O(1) space
 */
int	enqueue_input(t_input_queue *queue, int keycode);

/**
 * @brief Dequeue input event
 *
 * Removes and returns oldest event from input queue.
 *
 * @param queue Pointer to input queue
 * @param event Output parameter for dequeued event
 *
 * @return 1 on success, 0 if queue is empty
 *
 * @pre queue is non-NULL
 * @pre event is non-NULL
 * @post If queue not empty: event written to *event, tail advanced, count decremented
 * @post If queue empty: *event unchanged, returns 0
 *
 * Thread Safety: Not thread-safe
 * Complexity: O(1) time, O(1) space
 */
int	dequeue_input(t_input_queue *queue, t_input_event *event);

/**
 * @brief Process all queued inputs
 *
 * Dequeues and processes all inputs in queue. Called after render
 * completes to handle inputs that arrived during rendering.
 *
 * @param queue Pointer to input queue
 * @param render Pointer to render context (for handle_navigation_key)
 *
 * @pre queue is non-NULL
 * @pre render is non-NULL
 * @post Queue is empty (count=0)
 * @post All queued inputs processed via handle_navigation_key
 *
 * Thread Safety: Not thread-safe
 * Complexity: O(n) time where n=queue count (max 16), O(1) space
 */
void	process_input_queue(t_input_queue *queue, void *render);

/**
 * @brief Get current timestamp in microseconds
 *
 * Returns current time since epoch in microseconds using gettimeofday().
 *
 * @return Timestamp in microseconds (tv_sec * 1000000 + tv_usec)
 *
 * @post Return value > 0
 *
 * Thread Safety: Safe (system call)
 * Complexity: O(1) time, O(1) space
 */
long	get_timestamp_usec(void);

/**
 * State Machine Transitions
 *
 * Valid state transitions for debounce timer:
 *
 *   IDLE → PENDING:         First keyboard input received
 *   PENDING → PENDING:      New input resets timer
 *   PENDING → RENDERING:    Timer expired, start render
 *   RENDERING → IDLE:       Render complete, no queued inputs
 *   RENDERING → PENDING:    Render complete, process queued inputs
 *
 * Invalid transitions result in undefined behavior.
 */

#endif
