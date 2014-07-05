VALVE_DIAM = 21;
VALVE_DEPTH = 13;
WALL_THICKNESS = 4;
MIDSECTION = 5;
ORING_HEIGHT = 3;
OUTPUT_DIAM = 11;
OUTPUT_DEPTH = 7;

difference() {
  union() {
    difference() {
      cylinder(d=VALVE_DIAM+2*WALL_THICKNESS, h=VALVE_DEPTH+MIDSECTION+ORING_HEIGHT);
      translate([0,0,-0.1]) cylinder(d=VALVE_DIAM, h=VALVE_DEPTH+0.1);
      translate([0,0,VALVE_DEPTH+MIDSECTION]) cylinder(d=OUTPUT_DIAM+ORING_HEIGHT, h=ORING_HEIGHT*2);
    }
    translate([0,0,VALVE_DEPTH+MIDSECTION]) cylinder(d=OUTPUT_DIAM, h=OUTPUT_DEPTH+ORING_HEIGHT+1);
 }
 translate([0,0,VALVE_DEPTH-0.1]) cylinder(d=OUTPUT_DIAM-WALL_THICKNESS, h=OUTPUT_DEPTH+ORING_HEIGHT+MIDSECTION+0.2);
 translate([0,0,VALVE_DEPTH-0.1]) cylinder(d=OUTPUT_DIAM-WALL_THICKNESS, h=OUTPUT_DEPTH+ORING_HEIGHT+MIDSECTION+2);
}
