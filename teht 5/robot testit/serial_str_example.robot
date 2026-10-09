*** Settings ***
Library   String
Library   SerialLibrary

*** Variables ***
${com}   	COM8
${baud} 	115200
${board}	nRF
${seq}      RYGX
${error}    -1X
${time}		000120X
${time2}	0001200X
${expected}		80X
${expected2}		-1X
${time3}		00012aX
${expected3}	-5X

*** Test Cases ***
Connect Serial
	Log To Console  Connecting to ${board}
	Add Port  ${com}  baudrate=${baud}  timeout=2  encoding=ascii
	Port Should Be Open  ${com}
	Reset Input Buffer
	Reset Output Buffer

Time Parser Test
    # Lähetetään aika ja lopetusmerkki X
    Write Data   ${time}   encoding=ascii
    Log To Console   Send time ${time}
    # Luetaan vastaus X-merkkiin asti
    ${read} =   Read Until   terminator=58   encoding=ascii

    Log To Console   Received ${read}

    # Tarkistetaan että saatiin 80X
    Should Be Equal As Strings   ${read}   ${expected}

Time Parser Test Len
    # Lähetetään aika ja lopetusmerkki X
    Write Data   ${time2}   encoding=ascii
    Log To Console   Send time ${time2}
    # Luetaan vastaus X-merkkiin asti
    ${read} =   Read Until   terminator=58   encoding=ascii

    Log To Console   Received ${read}

    # Tarkistetaan että saatiin 80X
    Should Be Equal As Strings   ${read}   ${expected2}

Time Parser Test Digit
    # Lähetetään aika ja lopetusmerkki X
    Write Data   ${time3}   encoding=ascii
    Log To Console   Send time ${time3}
    # Luetaan vastaus X-merkkiin asti
    ${read} =   Read Until   terminator=58   encoding=ascii

    Log To Console   Received ${read}

    # Tarkistetaan että saatiin 80X
    Should Be Equal As Strings   ${read}   ${expected3}
	
Disconnect Serial
	Log To Console  Disconnecting ${board}
	[TearDown]  Delete Port  ${com}