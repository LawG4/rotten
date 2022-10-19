# Rotten Retro Gfx Simulation

So you want to write graphics for a retro system, and using an abstraction layer just won't cut it? Perhaps the performance impact of that layer means you're not going to get the speed you need. But testing for the system itself is slow and tedious. 

The solution we offer here is to simulate the exact graphics API on desktop, and then when you build natively for your retro platform you won't even need to change any of the API calls, basically sacrifice desktop prototyping performance for native graphics performance benefits in the final product!
