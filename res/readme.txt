Name (PRES, One)
        Method (_L6F, 0, NotSerialized)  // _Lxx: Level-Triggered GPE, xx=0x00-0xFF
        {
            If ((RTD3 == One))
            {
                If (CondRefOf (\_GPE.AL6F))
                {
                    AL6F ()
                }
            }

            If ((SGGP == One))
            {
                If (CondRefOf (\_GPE.P0L6))
                {
                    P0L6 ()
                }
            }

            If ((P1GP == One))
            {
                If (CondRefOf (\_GPE.P1L6))
                {
                    P1L6 ()
                }
            }

            If ((P2GP == One))
            {
                If (CondRefOf (\_GPE.P2L6))
                {
                    P2L6 ()
                }
            }
        }