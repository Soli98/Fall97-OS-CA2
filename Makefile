COMPILER = g++ -g

all : loadBalancer.cpp presenter.cpp worker.cpp
	${COMPILER} -o loadBalancer loadBalancer.cpp && ${COMPILER} -o presenter presenter.cpp && ${COMPILER} -o worker worker.cpp

loadBalancer : loadBalancer.cpp
	${COMPILER} -o loadBalancer loadBalancer.cpp

presenter : presenter.cpp
	${COMPILER} -o presenter presenter.cpp

worker : worker.cpp
	${COMPILER} -o worker worker.cpp

clean:
	rm *.o loadBalancer presenter worker