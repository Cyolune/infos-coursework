/*
 * The Priority Task Scheduler
 * SKELETON IMPLEMENTATION TO BE FILLED IN FOR TASK 1
 */

#include <infos/kernel/sched.h>
#include <infos/kernel/thread.h>
#include <infos/kernel/log.h>
#include <infos/util/list.h>
#include <infos/util/lock.h>

using namespace infos::kernel;
using namespace infos::util;

/**
 * A Multiple Queue priority scheduling algorithm
 */
class MultipleQueuePriorityScheduler : public SchedulingAlgorithm
{
public:
    /**
     * Returns the friendly name of the algorithm, for debugging and selection purposes.
     */
    const char* name() const override { return "mq"; }

    /**
     * Called during scheduler initialisation.
     */
    void init()
    {
        // TODO: Implement me!
    }

    /**
     * Called when a scheduling entity becomes eligible for running.
     * @param entity
     */
    void add_to_runqueue(SchedulingEntity& entity) override
    {
        // TODO: Implement me!
        UniqueIRQLock l;
        SchedulingEntityPriority::SchedulingEntityPriority priority = entity.priority();
        switch (priority) {
            case SchedulingEntityPriority::REALTIME:
                realtime_runqueue.enqueue(&entity);
                break;
            case SchedulingEntityPriority::INTERACTIVE:
                interactive_runqueue.enqueue(&entity);
                break;
            case SchedulingEntityPriority::NORMAL:
                normal_runqueue.enqueue(&entity);
                break;
            case SchedulingEntityPriority::DAEMON:
                daemon_runqueue.enqueue(&entity);
                break;
            default:
                // Should not reach here
                // syslog.messagef(LogLevel::ERROR, "thread %d has invalid priority while enqueuing.", entity);
                break;
        }
    }

    /**
     * Called when a scheduling entity is no longer eligible for running.
     * @param entity
     */
    void remove_from_runqueue(SchedulingEntity& entity) override
    {
        // TODO: Implement me!
        UniqueIRQLock l;
        SchedulingEntityPriority::SchedulingEntityPriority priority = entity.priority();
        switch (priority) {
            case SchedulingEntityPriority::REALTIME:
                realtime_runqueue.remove(&entity);
                break;
            case SchedulingEntityPriority::INTERACTIVE:
                interactive_runqueue.remove(&entity);
                break;
            case SchedulingEntityPriority::NORMAL:
                normal_runqueue.remove(&entity);
                break;
            case SchedulingEntityPriority::DAEMON:
                daemon_runqueue.remove(&entity);
                break;
            default:
                // Should not reach here
                // syslog.messagef(LogLevel::ERROR, "thread %d has invalid priority while removing.", entity);
                break;
        }
    }

    /**
     * Called every time a scheduling event occurs, to cause the next eligible entity
     * to be chosen.  The next eligible entity might actually be the same entity, if
     * e.g. its timeslice has not expired.
     */
    SchedulingEntity *pick_next_entity() override
    {
        // TODO: Implement me!
        UniqueIRQLock l;        
        int entity_count = realtime_runqueue.count() + interactive_runqueue.count() + normal_runqueue.count() + daemon_runqueue.count();

        if (entity_count == 0) {
            return NULL;
        }

        // There is at least 1 entity in either of the 4 runqueues.
        if (realtime_runqueue.count() > 0) {
            return round_robin_select(realtime_runqueue);
        } else if (interactive_runqueue.count() > 0) {
            return round_robin_select(interactive_runqueue);
        } else if (normal_runqueue.count() > 0) {
            return round_robin_select(normal_runqueue);
        } else {
            return round_robin_select(daemon_runqueue);
        }

    }

    /**
     * Returns the first entity in the runqueue, and adds it to the back of the queue.
     */
    SchedulingEntity* round_robin_select(List<SchedulingEntity *> &runqueue) {
        if (runqueue.count() == 1) {
            return runqueue.first();
        }

        SchedulingEntity* entity = runqueue.dequeue();
        runqueue.enqueue(entity);
        return entity;
    }


private:
	List<SchedulingEntity *> realtime_runqueue;
    List<SchedulingEntity *> interactive_runqueue;
	List<SchedulingEntity *> normal_runqueue;
	List<SchedulingEntity *> daemon_runqueue;


};

/* --- DO NOT CHANGE ANYTHING BELOW THIS LINE --- */

RegisterScheduler(MultipleQueuePriorityScheduler);