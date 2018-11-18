COMPILER = g++

all : header.h header.cpp loadBalancer.cpp presenter.cpp worker.cpp
	${COMPILER} -o loadBalancer header.cpp loadBalancer.cpp && ${COMPILER} -o presenter header.cpp presenter.cpp && ${COMPILER} -o worker header.cpp worker.cpp

loadBalancer : header.h header.cpp loadBalancer.cpp
	${COMPILER} -o loadBalancer header.cpp loadBalancer.cpp

presenter : header.h header.cpp presenter.cpp
	${COMPILER} -o presenter header.cpp presenter.cpp

worker : header.h header.cpp worker.cpp
	${COMPILER} -o worker header.cpp worker.cpp

clean:
	rm *.o loadBalancer presenter worker