

ARGUMENTS IF YOU WANT TO CHANGE MAKE FILE

./executable <philosopher_count> <MIN_THINK_TIME> <MAX_THINK_TIME> <MIN_EAT_TIME> <MAX_EAT_TIME> <exponential | uniform> <EAT_COUNT>


DEPENDENCIES

-lpthread —> Pthreads
-lm ——> log()


SOLUTION
• Used the pseudocode from the slides as guide• Implemented the solution with semaphores and mutex locks(mutex lock is a semaphore too justinitialized to 1).• Used mutex lock beginning and end of the take_fork and put_fork functions to provide mutualexclusion in those functions.• Used other semaphores for resource allocations. Allocate one semaphore for each fork ,initialise all to 0. Then call a sem_wait on the specific semaphore if philosopher achieves to take the fork. And call a sem_post on the specific semaphore if a philosopher puts down a fork.
GENERATING RANDOM VALUES
• Generated the random values every time for wait of think and wait of eat.• Generated the uniform distributed values by generating a random double between 0 andRAND_MAX(with rand() function) then taking its mod on range (MAX - MIN) than adding the MIN value.• Generated the exponential distributed values by generating a random double z, between 0 and 1. Then taking logarithm of z and multiplying it with the negative of mean. I found that method in the internet, its based on the inverse PDF function of exponential distribution.