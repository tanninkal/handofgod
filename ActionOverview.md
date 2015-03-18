Every entity will always be pursuing one of the 6 goals.  There is no downtime, but perceived downtime, travel, LFG, etc., must be built as an action of sufficient importance to not be interrupted without external stimulus or lack of attention span (always a viable reason.)

To this end, every entity will maintain an action-stack, with one of the 6 goals as the root.

Each action available to a character will be one of two types: a simple or a complex action.  A simple action will be implemented purely in code, while a complex action will be implemented by calling other actions.

A simple action might be "build a wall," comprised of cehcking necessary components, figuring out the line, maybe a property border (probably always a property border), and then procedurally constructing the wall, to be interrupted only by stimulus, such as an attack, or boredom.  A complex action could be anything from "farm some gold" to "kill the monster."

A sequence of actions might be:
  1. while building a wall, ADD syndrome wins, need something new.
  1. pick a goal:  ...kill
  1. kill script:
    * if(social\_check) group
    * find targets of appropriate level
    * kill them
  1. failed social check, going solo
  1. find target script:
    * if(check knowledge for existing) travel
    * else explore
  1. knowledge check for targets of appropriate difficulty
    * check risk calc, prefer moderate risk
  1. fought wolves NW of here that were easy at my skill level, reject
  1. fought orcs NNW of here that were very difficult at a previous skill level, accept
  1. passed knowledge check
  1. travel to the old/known location
  1. interrupt/stimulus: attacked by a wolf
  1. consider difficulty: knowledge check + wis check: probably easy, engage (else: run)
  1. fight!
> > `unsure whether combat should be a simple or complex action, whether to continue to script and let the simples be "heal", "slash" etc.  hmm.`
  1. fight script, very rough:
    * if (losing) { if (run\_check) run else if (heal\_check) heal}  //losing is a wis check assessment of reality
    * if (knowledge\_check->target) pick knowledge->target->combat\_option //if we know what worked on this mob, use it
    * else pick a combat action //simple action, picks based on random+classes, weighted based on own history of success
    * if (pet) repeat something similar for the pet's action.
    * simple action: swing, nuke, counter, whatever
> > `every turn will reevaluate current situation, some stateful information, probably like "i'm in combat" or "i have agro" `
  1. nuke
    * similar process to pick a spell
  1. cast spell "X" on target (or AE, etc)


Thus, the action stack before the combat might be:

GOAL: kill

> action: find level-appropriate encounter
> > action: travel to _history coordinates_  //pathing is a simple

and after the interrupt, at the end:

GOAL: kill

> action: combat
> > action: nuke
> > > action: cast "Ultimate DD Death Level 1" on "wolf pup" (simple, clearly)