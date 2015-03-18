
```
goal(6)
 goal-classes(several)
  helper-class(0+) //which classes most aid this one, not stored per-char

preferences(1)
 goal-specific(6)
  goal-pref
 uncategorized(lots)

attributes(sev)
 innate(sev)
  change-formula(1)
 derived(sev)
  basis-formula(1)
 resistance(sev)
  basis-formula(1)

actions(lots)
 goalclasses-advanced(1+)
 attribute-bases(1+)
 gear-bases(0+) //weapon, armor, shop?, etc.

histories(?)
 actions(1)
  action-taken(1)
  classes-affected(1+)
   weak-basis(0+)
   adequate(0+)
   strength(0+)
   advanced(#)
 lists(1)
  combat(0+)
   target(0+)
    myskill(1+)
     success(0-100)
  social(0+)
   target(0+)
    //some qualitative judge
    combat(0-1)
     effectiveness(1)
    ...others
  trade
   item(0+)
    sold/bought/saw(0+)
     cost(1)
    item information/perceived value(1)
  explore
   location(0+) //defining this will be exciting
    combat(0+)
     levels(1+)
     types(1+)
     numbers(1+)
    noncombat(0+)
     levels(1+)
     types(1+)
     numbers(1+)
    explore(0+)
     //um, adjoining areas

physical
 inherent-weapon(0+)
  weapon-type(1+)
 gear-slots(0+)
  gear-type(1+)
 goal-potential(0-6)  //whether this ent can pursue/advance in various goals
  class-potential(1+)

```