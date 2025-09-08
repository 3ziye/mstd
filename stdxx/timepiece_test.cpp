// build command cl /DWIN32 timepiece_test.cpp threadpool.cpp system.cpp timepiece.cpp
#include <iostream>
#include "threadpool.h"
#include "system.h"
#include "timepiece.h"

void foo()
{
	std::cout << "This is foo" << std::endl;
}

void foo2()
{
	stdxx::sleep(1000);
	std::cout << "This is foo2" << std::endl;
}

int main()
{

	stdxx::timepiece t;
	t.start();

	stdxx::threads::pool pool;
	for (int i = 0; i < 10; ++i)
		pool.push(foo);
	pool.signal();          //Make a signal and the signal will be triggered when the
							//tasks which are pushed before it are finished.
	pool.push(foo2);
	pool.wait_for_signal(); //Wait until the signal is triggered. The wait might be finished
							//before finish of foo2, because the signal is made before pushing foo2.

	std::cout << "timepiece ms: " << t.calc() << std::endl;
}
