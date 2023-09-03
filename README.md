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

https://cdn.masto.host/mastodongamedevplace/media_attachments/files/110/134/102/398/679/883/original/23d9074bd3c31bb8.mp4

Second video is showing the updating of the flowfield in more detail:

https://cdn.masto.host/mastodongamedevplace/media_attachments/files/110/113/019/350/180/454/original/b54615b764493cd4.mp4

And here is a screenshot with "distance to target" shown:

https://cdn.masto.host/mastodongamedevplace/media_attachments/files/110/125/954/378/696/175/original/45a618febb6c7950.png

I'll try to provide a simple documentation sometime.

# Stats
From some experiments back then (I'll have to redo testing, but this is what I wrote down):

Calculating a flowfield for an empty 30 by 40 cell map:
GDScript ~6ms
C++ (after optimization): 0.8-1ms