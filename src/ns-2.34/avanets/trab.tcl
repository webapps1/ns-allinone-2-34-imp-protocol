# Define options 
set val(chan)          Channel/WirelessChannel      ;# channel type
set val(prop)          Propagation/TwoRayGround     ;# radio-propagation model
set val(netif)         Phy/WirelessPhy/802_15_4     ;# network interface type
set val(mac)           Mac/802_15_4                 ;# MAC type
set val(ifq)           Queue/DropTail/PriQueue      ;# interface queue type
set val(ll)            LL                           ;# link layer type
set val(ant)           Antenna/OmniAntenna          ;# antenna model
set val(ifqlen)        100	         	    ;# max packet in ifq
set val(nn)            5			    ;# number of mobilenodes XFXVanets
set val(rp)            XFXVanets       		    ;# protocol tye
set val(x)             120			    ;# X dimension of topography
set val(y)             120			    ;# Y dimension of topography
set val(stop)          500			    ;# simulation period 
set val(energymodel)   EnergyModel		    ;# Energy Model
set val(initialenergy) 100			    ;# value

 
set ns		  [new Simulator] 
set tracefd       [open simple.tr w] 
set windowVsTime2 [open win.tr w]  
set namtrace      [open simwrls.nam w]     
 
$ns trace-all $tracefd 
$ns namtrace-all-wireless $namtrace $val(x) $val(y) 
 
# set up topography object 
set topo       [new Topography] 
 
$topo load_flatgrid $val(x) $val(y) 
 
create-god $val(nn) 
 
# 
#  Create nn mobilenodes [$val(nn)] and attach them to the channel.  
# 
 
# configure the nodes 
# configure the nodes
$ns node-config -adhocRouting $val(rp) \
	    -llType $val(ll) \
	     -macType $val(mac) \
	     -ifqType $val(ifq) \
	     -ifqLen $val(ifqlen) \
	     -antType $val(ant) \
	     -propType $val(prop) \
	     -phyType $val(netif) \
	     -channel [new $val(chan)] \
	     -topoInstance $topo \
	     -agentTrace ON \
	     -routerTrace ON \
	     -macTrace  OFF \
	     -movementTrace OFF \
	     -energyModel $val(energymodel) \
	     -initialEnergy $val(initialenergy) \
	     -rxPower 35.28e-3 \
	     -txPower 31.32e-3 \
	     -idlePower 712e-6 \
	     -sleepPower 144e-9 
			   
			  
for {set i 0} {$i < $val(nn) } { incr i } {
      set mnode_($i) [$ns node]
}

## seta as rotas
$ns at 1.5 "$mnode_(0) route 0.0 1.0 0.0 12.0 13.0 0.0"
$mnode_(0) set X_ 0.0
$mnode_(0) set Y_ 1.0
$mnode_(0) set Z_ 0
$ns at 1.5 "$mnode_(1) route 1.0 3.0 0.0 15.0 16.0 0.0"
$mnode_(1) set X_ 1.0
$mnode_(1) set Y_ 3.0
$mnode_(1) set Z_ 0
$ns at 1.5 "$mnode_(2) route 2.0 3.0 0.0 10.0 12.0 0.0"
$mnode_(2) set X_ 2.0
$mnode_(2) set Y_ 3.0
$mnode_(2) set Z_ 0
$ns at 1.5 "$mnode_(3) route 3.0 4.0 0.0 14.0 15.0 0.0"
$mnode_(3) set X_ 3.0
$mnode_(3) set Y_ 4.0
$mnode_(3) set Z_ 0
$ns at 1.5 "$mnode_(4) route 4.0 5.0 0.0 17.0 18.0 0.0"
$mnode_(4) set X_ 4.0
$mnode_(4) set Y_ 5.0
$mnode_(4) set Z_ 0
#-- seta os destinos
$ns at 3.5 "$mnode_(0) setdest 12.0 13.0 0.0"
$ns at 4.5 "$mnode_(1) setdest 15.0 16.0 0.0"
$ns at 5.5 "$mnode_(2) setdest 10.0 12.0 0.0"
$ns at 3.5 "$mnode_(3) setdest 14.0 15.0 0.0"
$ns at 4.5 "$mnode_(4) setdest 17.0 18.0 0.0"

$ns run