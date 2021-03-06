.. py:class:: ExecutableModel(*args, **kwargs)
   :module: roadrunner

.. method:: ExecutableModel.evalInitialConditions()
   :module: roadrunner
   
.. method:: ExecutableModel.getBoundarySpeciesAmounts(*args)
   :module: roadrunner
   
   Returns a vector of boundary species amounts. The order of species is
   given by the order of Ids returned by getBoundarySpeciesIds()
      
   :param numpy.ndarray index: (optional) an index array indicating which items to return.
   :returns: an array of the boundary species amounts.
   :rtype: numpy.ndarray.
      
.. method:: ExecutableModel.getBoundarySpeciesConcentrations(*args)
   :module: roadrunner
      
   Returns a vector of boundary species concentrations. The order of species is
   given by the order of Ids returned by getBoundarySpeciesIds()
   
   :param numpy.ndarray index: (optional) an index array indicating which items to return.
   :returns: an array of the boundary species concentrations. 
   :rtype: numpy.ndarray.
      
      
   
.. method:: ExecutableModel.getBoundarySpeciesIds()
   :module: roadrunner
   
   Returns a vector of boundary species Ids.
      
   :param numpy.ndarray index: (optional) an index array indicating which items to return.
   :returns: a list of boundary species ids.
      
.. method:: ExecutableModel.getCompartmentIds()
   :module: roadrunner
   
   Returns a vecotr of compartment Ids.
     
:param numpy.ndarray index: (optional) an index array indicating which items to return.
   :retuns: a list of compartment ids.
      
.. method:: ExecutableModel.getCompartmentVolumes(*args)
   :module: roadrunner
      
   Returns a vector of compartment volumes. The order of volumes is
   given by the order of Ids returned by getCompartmentIds()
   
   :param numpy.ndarray index: (optional) an index array indicating which items to return.
   :returns: an array of compartment volumes.
   :rtype: numpy.ndarray.
      
      
   
.. method:: ExecutableModel.getConservedSumChanged()
   :module: roadrunner
   
              
      
   
.. method:: ExecutableModel.getConservedSums(*args)
   :module: roadrunner
   
   :param numpy.ndarray index: (optional) an index array indicating which items to return.
   :returns: an array of conserved sums.
   :rtype: numpy.ndarray.
      
      
   
.. method:: ExecutableModel.getFloatingSpeciesAmountRates(*args)
   :module: roadrunner
   
   :param numpy.ndarray index: (optional) an index array indicating which items to return.
   :returns: an array of the rates of change of the floating species amounts.
   :rtype: numpy.ndarray
      
      
   
.. method:: ExecutableModel.getFloatingSpeciesAmounts(*args)
   :module: roadrunner
   
   Get the list of floating species amounts. If no arguments are given, this
   returns all floating species amounts.
      
   :param index: an optional array of desired floating species indices. i.e. if this model has
                 4 floating species and we want the amounts for the last and first, we
                 would use [3,0] for the index array.
   :type name: numpy.ndarray
      
   get all the amounts::
      
     >>> e.getFloatingSpeciesAmounts()
     [15,2,3,20]
      
   get amounts 3 and 0::
      
     >>> getFloatingSpeciesAmounts([3,0])
     [10,15]
      
      
      
   
.. method:: ExecutableModel.getFloatingSpeciesConcentrations(*args)
   :module: roadrunner
   
   Returns a vector of floating species concentrations. The order of species is
   given by the order of Ids returned by getFloatingSpeciesIds()
      
   :param numpy.ndarray index: (optional) an index array indicating which items to return.
   :returns: an array of floating species concentrations.
   :rtype: numpy.ndarray
      
      
   
.. method:: ExecutableModel.getGlobalParameterValues(*args)
   :module: roadrunner
   
   Return a vector of global parameter values. The order of species is
   given by the order of Ids returned by getGlobalParameterIds()
      
   :param numpy.ndarray index: (optional) an index array indicating which items to return.
   :returns: an array of global parameter values.
   :rtype: numpy.ndarray.
      
      
   
.. method:: ExecutableModel.getInfo()
   :module: roadrunner
   
              
      
   
.. method:: ExecutableModel.getModelName()
   :module: roadrunner
   
   Get the model name specified in the SBML.
      
      
   
.. method:: ExecutableModel.getNumBoundarySpecies()
   :module: roadrunner
   
   Returns the number of boundary species in the model.         
      
   
.. method:: ExecutableModel.getNumCompartments()
   :module: roadrunner
   
   Returns the number of compartments in the model.          
      
   
.. method:: ExecutableModel.getNumConservedSums()
   :module: roadrunner
   
   TODO DOCS         
      
   
.. method:: ExecutableModel.getNumDependentSpecies()
   :module: roadrunner
   
   Returns the number of dependent floating species in the model.          
      
   
.. method:: ExecutableModel.getNumFloatingSpecies()
   :module: roadrunner
   
   Returns the number of floating species in the model.         
       
   
.. method:: ExecutableModel.getNumGlobalParameters()
   :module: roadrunner
   
              
   Returns the number of global parameters in the model.
     
   
.. method:: ExecutableModel.getNumIndependentSpecies()
   :module: roadrunner
   
   Returns the number of independent floating species in the model.       
      
   
.. method:: ExecutableModel.getNumReactions()
   :module: roadrunner
   
   Returns the number of reactions in the model.
   
.. method:: ExecutableModel.getNumRules()
   :module: roadrunner
   
   Returns the number of rules in the SBML model.       
      
   
.. method:: ExecutableModel.getReactionIds()
   :module: roadrunner
      
   Returns a vector of reaction Ids.
   
   :param numpy.ndarray index: (optional) an index array indicating which items to return.
   :returns: a list of reaction ids.
      
      
   
.. method:: ExecutableModel.getReactionRates(*args)
   :module: roadrunner
   
   Returns a vector of reaction rates for the current state of the model. The order of 
   reaction rates is given by the order of Ids returned by getReactionIds()
      
   :param numpy.ndarray index: (optional) an index array indicating which items to return.
   :returns: an array of reaction rates.
   :rtype: numpy.ndarray
      
      
   
.. method:: ExecutableModel.getTime()
   :module: roadrunner
   
   Not sure what this does        
      
   
.. method:: ExecutableModel.reset()
   :module: roadrunner
   
   Reset the floating species concentration to their initial conditions.        
      
   
.. method:: ExecutableModel.setBoundarySpeciesConcentrations(*args)
   :module: roadrunner
   
   Use this to set the entire set of boundary species concentrations in one call. 
   The order of species is given by the order of Ids returned by getBoundarySpeciesIds()
      
   :param numpy.ndarray index: (optional) an index array indicating which items to set, 
                               or if no index array is given, the first param should be an 
                               array of all the  values to set.

   :param numpy.ndarray values: the values to set.
      

.. method:: ExecutableModel.setCompartmentVolumes(*args)
   :module: roadrunner
   
   Use this to set the entire set of compartment volumes in one call. 
   The order of compartments is given by the order of Ids returned by getCompartmentIds()
   
   :param numpy.ndarray index: (optional) an index array indicating which items to set, 
                               or if no index array is given, the first param should be an 
                               array of all the  values to set.
   :param numpy.ndarray values: the values to set.
      
      

   
.. method:: ExecutableModel.setConservedSumChanged(*args)
   :module: roadrunner

      
   
.. method:: ExecutableModel.setConservedSums(*args)
   :module: roadrunner
   
   :param numpy.ndarray index: (optional) an index array indicating which items to set, 
                               or if no index array is given, the first param should be 
                               an array of all the values to set.
   :param numpy.ndarray values: the values to set.
      
      
      
   
.. method:: ExecutableModel.setFloatingSpeciesAmounts(*args)
   :module: roadrunner
   
   Use this to set the entire set of floating species amounts in one call. 
   The order of species is given by the order of Ids returned by getFloatingSpeciesIds()
   
   :param numpy.ndarray index: (optional) an index array indicating which items to set, 
                               or if no index array is given, the first param should be 
                               an array of all the values to set.
   :param numpy.ndarray values: the values to set.
      
      
   
.. method:: ExecutableModel.setFloatingSpeciesConcentrations(*args)
   :module: roadrunner
      
   Use this to set the entire set of floating species concentrations in one call. 
   The order of species is given by the order of Ids returned by getFloatingSpeciesIds()
      
   
   :param numpy.ndarray index: (optional) an index array indicating which items to set, 
                               or if no index array is given, the first param should be an 
                               array of all the  values to set.
   :param numpy.ndarray values: the values to set.
      
      
   
.. method:: ExecutableModel.setGlobalParameterValues(*args)
   :module: roadrunner
      
   Use this to set the entire set of global parameters in one call. 
   The order of parameters is given by the order of Ids returned by getGlobalParameterIds()
      
   
   :param numpy.ndarray index: (optional) an index array indicating which items to set, 
                               or if no index array is given, the first param should be an 
                               array of all the values to set.
   :param numpy.ndarray values: the values to set.
      
      
   
.. method:: ExecutableModel.setTime(time)
   :module: roadrunner
   
   Set the model time variable. NOt sure what this does.
      
   :param time: time the time value to set.
   :returns: None
