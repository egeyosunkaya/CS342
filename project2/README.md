
Ege Yosunkaya 21402025
Serhan Gürsoy 21400840


\** Project Specifications in project2.pdf 



# -- -- -- Create Thread -- -- --
 It creates a new thread and adds to the queue. 
 The most challangeing thing was creating and initializing the stack.
 We created a new stack , make the stack pointer to point to the top of stack
 then made a new pointer to put tramboline address, function and paramaters. 
 Then we passed the second pointer that we created to REG_ESP.
 
# -- -- -- Yield Thread -- -- -- 
 It gives CPU to another thread
 We made a binary variable to understand if its first or second return of getcontext.
 We made diffrent cases for TLIB_SELF , TLIB_ANY or ordinary numbers.
 TLIB_ANY yields to the head of the ready queue. (In this case thread 1 instead of thread 0 even though head of the ready   queue is always the thread 0, thread 0 is not a real thread it only holds the values for user program.)
 
# -- -- -- Delete Thread -- -- -- 
 It deletes the thread , and gives CPU to the head of the ready queue
 


