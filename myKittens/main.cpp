/*
 * myKittens
 */
 
#include <sifteo.h>
#include "assets.gen.h"
using namespace Sifteo;

///////////////////////////////////////////////////////
// META DATA
///////////////////////////////////////////////////////
static AssetSlot MainSlot = AssetSlot::allocate()
.bootstrap(KittenAssets);

static AssetSlot SecondarySlot = AssetSlot::allocate()
.bootstrap(KittenAssets2);
	
static Metadata M = Metadata()
    .title("myKittens")
    .package("com.sifteo.sdk.mykittens", "0.1")
    .icon(Icon)
    .cubeRange(0, CUBE_ALLOCATION);

// asset related
static AssetLoader loader;
static AssetConfiguration<1> config;
static TiltShakeRecognizer motion[CUBE_ALLOCATION];
static VideoBuffer vid[CUBE_ALLOCATION];
///////////////////////////////////////////////////////










/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SENSOR LISTENER CLASS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SensorListener {
public:

struct Counter
{
	unsigned touch;
	unsigned neighborAdd;
	unsigned neighborRemove;
} counters[CUBE_ALLOCATION];


///////////////////////////////////////////////////////
// INITIALIZE
///////////////////////////////////////////////////////
void init(CubeID cube)
{
    frame = 0;
    vbuf.initMode(BG0_SPR_BG1);
    vbuf.attach(cube);
	
	Events::cubeAccelChange.set(&SensorListener::onAccelChange, this);
	Events::cubeTouch.set(&SensorListener::onTouch, this);
}
///////////////////////////////////////////////////////
	

///////////////////////////////////////////////////////
// UPDATE
///////////////////////////////////////////////////////
void update(TimeDelta timeStep, CubeID cube)
{		
	float millisPerFrame = .5;
	int nextFrame = SystemTime::now().cycleFrame(millisPerFrame, Idle.numFrames());
    if (nextFrame != frame) {
        vbuf.sprites[0].setImage(Idle, nextFrame);
        frame = nextFrame;
    }
	//frame++;
	
	// activate other states please!
	onTouch(cube);
	onAccelChange(cube);
	hasNeighbor(cube);
}
///////////////////////////////////////////////////////
 

private:
	unsigned frame;
	VideoBuffer vbuf;
///////////////////////////////////////////////////////
// TOUCHING
///////////////////////////////////////////////////////
void onTouch(unsigned id)
{    
	CubeID cube(id);
	
	// if you touch the cube
	if (cube.isTouching())
	{
		// kitten jumps!
		vbuf.sprites[0].setImage(Jump, frame % Jump.numFrames());
		frame++;
	}
	
}
///////////////////////////////////////////////////////


///////////////////////////////////////////////////////
// ACCELERATION, SHACK, AND TILT
///////////////////////////////////////////////////////
 void onAccelChange(unsigned id)
{
	CubeID cube(id);
    auto accel = cube.accel();
	
	if(accel.x < 0 || accel.y < 0)
	{
	// kitten is annoyed!
	vbuf.sprites[0].setImage(Annoyed, frame % Annoyed.numFrames() );
	frame++;
	}
	
	if(accel.z < 64)
	{
	// kitten is scared
	vbuf.sprites[0].setImage(Scared, frame % Scared.numFrames());
	frame++;
	}
}
///////////////////////////////////////////////////////


///////////////////////////////////////////////////////
// A CUBE HAS A NEIGHBOR
///////////////////////////////////////////////////////
void hasNeighbor(unsigned id)
{ 
	CubeID cube(id);
    auto neighbors = vid[id].physicalNeighbors();
	
	for(int side=0; side<4; ++side) 
	{
		if (neighbors.hasNeighborAt(Side(side))) 
		{
			vbuf.sprites[0].setImage(Playful, frame % Playful.numFrames());
			frame++;
		} 
	}
}
///////////////////////////////////////////////////////
};










///////////////////////////////////////////////////////
// MAIN IS LIKE SETUP, I THINK
///////////////////////////////////////////////////////
void main()
{	

// play music
AudioTracker::play(Music);
	
// sensors
static SensorListener sensors;
static SensorListener instances[CUBE_ALLOCATION];

// run loop
for (unsigned i = 0; i < arraysize(instances); i++)
instances[i].init(i);
		
TimeStep ts;
	while (1) 
	{
        for (unsigned i = 0; i < arraysize(instances); i++)
        instances[i].update(ts.delta(), i);
        System::paint();
        ts.next();
    }
	
}
///////////////////////////////////////////////////////
