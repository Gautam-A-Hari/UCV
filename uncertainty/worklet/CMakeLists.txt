##============================================================================
##  Copyright (c) Kitware, Inc.
##  All rights reserved.
##  See LICENSE.txt for details.
##
##  This software is distributed WITHOUT ANY WARRANTY; without even
##  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##  PURPOSE.  See the above copyright notice for more information.
##============================================================================

set(headers
  Fiber.h
  Fiber4Var.h
  #FiberMultiVar.h
  MeanField2Var.h
  Fiber2Var.h
  Fiber3Var.h
  )
#-----------------------------------------------------------------------------#i
vtkm_declare_headers(${headers})
