# One-Way-Tunnel-Multithreading

CSCI 144 - Project Report

For the project I created 3 threads which each simulate and run certain things. The first thread we called carNorth which is supposed to produce random number of cars in the north direction. Second thread is carSouth which involves in producing random number of cars in the south direction. Finally, we have a thread called passThroughTunnel which passes the cars through the tunnel.

The program runs the threads but getting the number of cars to show up is tricky. I also tried to implement a random number of cars but seems to give me a large amount when running it. Another thing wrong with the code is that the sleep function waits 60 seconds but doesn’t seem to increase the number of cars during that time. I messed with it a bit but it simply doesn’t allow me to, I tried adding a counter to keep track but it further messed up the code so I decided to get rid of it. 

Logically speaking both threads producing a random number of cars will produce cars in a while loop. As I mentioned before using rand()%121 would mess up the code so I went with something rather small such as rand()%10. Another issue with the code is that it appears to only be running the north side of the tunnel but not the south side of it. For the graph I was not able to come up with something since the code did not run properly. It would run but the outputs are not what we expected. Based on the output it seems it runs rather on an infinite loop with the number of cars increasing by only a few amounts. It also has function called processCar which is supposed to remove the cars simulating that they have successfully passed the tunnel by getting the front of the list and basically checking it off the list. This repeats throughout the list of cars by changing the head each time a car comes out of the tunnel. 

Image of Results
  
Video of Program Running
https://photos.app.goo.gl/Zw4FVapoRxBYzZyBA 
