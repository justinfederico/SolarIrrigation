# SolarIrrigation
This repository will have all firmware related resources pertaining to Team 8's Capstone project.

What is in this repo is subject to change, but as of writing it is a PlatformIO project designed for the Arduino Uno.
The eventual goal is the amalgamation of every subsystem into a single PCB, if possible.
Below is a state machine that is *IN PROGRESS*, so it is subject to change as our project develops.


State Machine:
```mermaid
stateDiagram-v2
        state Standby
        [*] --> Standby
        Standby --> MPPT: isSunny==true
        Standby --> Sleep: isSunny==false
        state MPPT {
        [*] --> Charging
        Charging --> NotCharging : batFull == true
        NotCharging --> Charging : batFull == false
        --
        [*] --> PumpIdle
        PumpIdle --> PumpWater : reservoirFull == false && suffBat == true
        PumpWater --> PumpIdle : reservoirFull == true
        --
        state DisplayData <<fork>>
        [*] --> DisplayData
        DisplayData --> SolarPanelInfo
        DisplayData --> BatteryInfo
        state BatteryInfo {
            [*] --> checkBat 
            state checkBat <<choice>>
            checkBat --> sufBat=false: if batLevel < 10%
            checkBat --> sufBat=true: if  99% > batLevel > 10%
            checkBat --> batFull=true: if batLevel > 99%
            sufBat=false --> displayBat
            sufBat=true --> displayBat: batFull=false
            batFull=true --> displayBat
            displayBat --> checkBat: Refresh Display
        }
        state SolarPanelInfo {
        [*] --> Read
        Read --> readVoltage
        Read --> readCurrent
        Read --> solarWatts
        readVoltage --> readVoltage : ADC 1
        readCurrent --> readCurrent : H.E. Sensor
        solarWatts --> solarWatts : P = I*V
        state refreshDisplay <<join>> 
        readVoltage --> refreshDisplay
        readCurrent --> refreshDisplay
        solarWatts --> refreshDisplay
        refreshDisplay --> ShowData
        ShowData --> Read : Refresh Display
        }

        
    }
    state Full <<join>>
    NotCharging --> Full
    PumpIdle --> Full
    Full --> Standby
    Sleep --> Standby: isSunny==true
        
        
    
    

```