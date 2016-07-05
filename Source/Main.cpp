
#include "Application.h"
#include "TestScene.h"
#include "OBJScene.h"
#include "SP3Scene.h"
#include "SP6Scene.h"
#include "BlockScene.h"

int main(int argc, char* argv[])
{
	Application app;

	//app.AddScene("Test", new TestScene());
	//app.AddScene("OBJ", new OBJScene());
	//app.AddScene("SP3", new SP3Scene());
	//app.AddScene("SP6", new SP6::SP6Scene());
	app.AddScene("Block", new Block::BlockScene());
	app.SetActiveScene("Block");

	if (app.Init())
	{
		app.Run();
		app.Shutdown();
	}

	return 0;
}

/*
1 byte per block for ids
1 byte per block for metadata
0 - 1.5KB per block for 16x16 8bit decal map(256 bytes per side)
0 - N bytes per block for tile entity data


Block ID - an array of 16x512x16, stored for every block, 131,072 block ids = 128KB
Metadata - struct { 4bit x, 4bit z, 9bit y, 7bit meta }, 2 bytes per metadata, stored only for blocks that actually have metadata, so maybe 1/10th blocks have metadata = 25.6KB

Decal maps - struct { 4bit x, 4bit z, 9bit y, 3bit side, 256B decal }, 258 bytes per decal, stored only for blocks that actually have a decal,
	and only for sides that have a decal.
	If every single block on one surface layer has 3 exposed sides, 16x16x3 = 768 exposed sides per layer
	With a height of 512, the most layers there could be in a chunk would be roughly 30 layers = 30x768 = 23,040 sides per chunk = 5.66MB
	More reasonable 10 layers = 7680 sides per chunk = 1.88964MB

Tile entity data - struct { 4bit x, 4bit z, 9bit y, NB data }, size per tile entity is unknown, depends on tile entity, but only stored for actual tile entities
	As with metadata, likely there will only be 1/10th of the blocks will be tile entities per chunk, likely it might even be as low as 1/512th, but I can't imagine
	it being higher then 1/64th, which would mean, for every block across a chunk(X and Z), there is 8 tile entities
	Low case(1/512), 256 tile entities, 64 bytes per TE = 16KB
	High case(1/64), 2048 tile entities, 64 bytes per TE = 128KB
	Higher case(1/64, 128B per) = 256KB
	Extreme (1/10, 256B per) = 3.2MB

Considering View distance of 20 chunks, which means 41x41 chunks in memory, 1681 chunks
	Block ID = 210.125MB
	Metadata = 42.025MB
	Decals = 9.29GB
	TE(extreme case) = 5.25GB

Lets consider that for decals, we only need the data from up to 5 chunks away, which means 11x11 chunks, 121 chunks for decal data
	Decals(11x11 chunks) = 684.86MB

Do the same for tile entities
	TE(Extreme case, 11x11 chunks) = 387.2MB

Total(Extreme case, per player) = 1324.21MB


Lets do a more reasonable case, same view distance of 41x41 chunks, 1681 chunks
	Block ID = 210.125MB
	Metadata = 42.025MB
	Decals(3x3 chunks, 10 layers) = 17MB
	TE(High case, 11x11 chunks) = 15.125MB

Total(High case, per player) = 284.275MB


Now for the world, of lets say 200x200 chunks, 40,000 chunks, use more reasonable figures rather then the extremes
	Block ID = 4.88GB
	Metadata = 0.97GB
	Decals(10 layers) = 73.81GB
	TE(high case) = 4.88GB

This is still a pretty extreme case, but it shows that the decals use up exponentially more data then anything else
Thankfully, decals only appear when the side of a block is directly interacted with, which means only a very small portion
of the world will even have decals, and any sides which are not visible, cannot have decals on them, which means we have a very large portion of the following:
	Air blocks(0 ID) cannot have any decals, and take up a very large percentage of the world, lets say 50% = air
	Block completely surrounded by other blocks cannot have any decals, and take up a large percentage of the world, lets say 25%
	Blocks that only have 1 side exposed to air, so can only have 1 decal, this is maybe 15%
	Blocks that have 2 sides exposed, this will happen on walls(opposite sides exposed) and edges(2 joining sides exposed), this is maybe 5%
	Blocks that have 3 sides exposed, corner blocks(3 joining sides exposed), wall tops(single block thick walls), probably about 4%
	Blocks that have 4 or more sides exposed, this would be very rare naturally, so probably only about 1% of blocks have this many sides exposed

	So in a 200x200 chunk world, made up of 5,242,880,000 blocks:
		Air/surrounded blocks: 3,932,160,000 blocks with no decals
		1 side exposed: 786,432,000 blocks with 1 side exposed = 786,432,000 sides
		2 sides: 262,144,000 blocks with 2 sides exposed = 524,288,000 sides
		3 sides: 209,715,200 blocks with 3 sides exposed = 629,145,600 sides
		4-6 sides: 52,428,800 blocks with 4-6 sides exposed = 209,715,200 - 314,572,800 sides
		
		Total sides: 2,149,580,800 - 2,254,438,400
		Avg: 2,202,009,600

	Considering most blocks will not have any decals, even out of the blocks with exposed sides, a generous number would be 10% of exposed sides have decals:
		10% with decals = 220,200,960 decals = 52.91GB
		More realistic 1% = 22,020,096 decals = 5.29GB



16x512x16 blocks per chunk = 131,072 blocks per chunk
2B - 2KB per block

262,144B - 268,435,456B per chunk
256KB - 256MB per chunk

200x200 chunk world = 9GB - 9TB


16x256x16 blocks per chunk = 65,536 blocks per chunk
2B - 2KB per block

131,072B - 134,217,728B per chunk
128KB - 128MB per chunk

200x200 chunk world = 5000MB - 5000GB
*/