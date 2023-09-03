# Thanks
Thanks to paddy-exe and their GDExtension Template (https://github.com/paddy-exe/GDExtensionSummator)

# GodotFlowfield
A Flowfield Pathfinding implementation for Godot. It is very barebones and was only used in my currently on-hold tower-defense game.

Some limitations:
- No variable costs
- No negative costs
- Only works in positive coordinates (0,0 -> towards positive) due to naive array access for speedup
- Quite optimized

# Video
Here is a video of it "in action". First video was a sort of "speed test". I randomly generate a tower every x ms (don't remember right now, will look up) and place it randomly on the map. Then the flowfield checks if the tile the tower is supposed to be placed on is free.

Free here means:
- Not blocking the remaining path for enemies
- There is nothing else already built there


Uploading 23d9074bd3c31bb8.mp4…


Second video is showing the updating of the flowfield in more detail:


Uploading b54615b764493cd4.mp4…


And here is a screenshot with "distance to target" shown:

![Uploading 45a618febb6c7950.png…]()


I'll try to provide a simple documentation sometime.
