.. py:class:: SelectionRecord(*args)
   :module: roadrunner

   RoadRunner provides a range of flexible ways of selecting values from 
   a simulation. These values can not only be calculated directly via
   RoadRunner.getSelectionValue, but any of these selections can be
   used as columns in the simulate result matrix. 
   
   The SectionRecord.selectionType should be one of the constants listed
   here. 
   
   Most selection types only require the first symbol id, p1 to be set, 
   however certain ones such as [???] require both p1 and p2.
   
   
   
   .. py:attribute:: SelectionRecord.index
      :module: roadrunner
      :annotation: int
   
   
   .. py:attribute:: SelectionRecord.p1
      :module: roadrunner
      :annotation: str
   
   
   .. py:attribute:: SelectionRecord.p2
      :module: roadrunner
      :annotation: str
   
   
   .. py:attribute:: SelectionRecord.selectionType
      :module: roadrunner
      :annotation: int
   
