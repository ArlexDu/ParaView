/*=========================================================================

  Program:   ParaView
  Module:    vtkPVImplicitPlaneRepresentation.cxx

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkPVImplicitPlaneRepresentation.h"

#include "vtkObjectFactory.h"
#include "vtkMultiProcessController.h"
#include "vtkProperty.h"
#include "vtkTransform.h"

vtkStandardNewMacro(vtkPVImplicitPlaneRepresentation);
//----------------------------------------------------------------------------
vtkPVImplicitPlaneRepresentation::vtkPVImplicitPlaneRepresentation()
{
  vtkMultiProcessController * ctrl = NULL;
  ctrl = vtkMultiProcessController::GetGlobalController();
  double opacity = 1;
  if(ctrl == NULL || ctrl->GetNumberOfProcesses() == 1)
    {
    opacity = 0.25;
    }

  this->OutlineTranslationOff();
  this->GetPlaneProperty()->SetOpacity(opacity);
  this->GetSelectedPlaneProperty()->SetOpacity(opacity);


  //create and connect the two transforms together
  this->Transform = vtkTransform::New();
  this->InverseTransform = vtkTransform::New();
  this->Transform->PostMultiply();
  this->Transform->Identity();  
  this->InverseTransform->SetInput(this->Transform);
  this->InverseTransform->Inverse();
}

//----------------------------------------------------------------------------
vtkPVImplicitPlaneRepresentation::~vtkPVImplicitPlaneRepresentation()
{
  this->InverseTransform->Delete();
  this->Transform->Delete();;
}

//----------------------------------------------------------------------------
void vtkPVImplicitPlaneRepresentation::SetTransform(vtkTransform *transform)
{
  if ( transform )
    {
    this->Transform->SetInput(transform);
    this->InverseTransform->Update();
    
    
    this->PlaceTransformedWidget(this->GetBounds());

    double origin[3];
    this->GetOrigin(origin);
    this->SetTransformedOrigin(origin[0],origin[1],origin[2]);
    this->UpdatePlacement();
    }
}

//----------------------------------------------------------------------------
void vtkPVImplicitPlaneRepresentation::SetTransformedOrigin(double x, double y, double z)
{
  double in_point[4] = {x, y, z, 1};
  this->Transform->TransformPoint(in_point, in_point);  
  this->Superclass::SetOrigin(in_point);
}

//----------------------------------------------------------------------------
void vtkPVImplicitPlaneRepresentation::SetTransformedNormal(double x, double y, double z)
{    
  double in_point[3]={x,y,z};
  this->Transform->TransformNormal(in_point,in_point);
  this->Superclass::SetNormal(in_point);
}

//----------------------------------------------------------------------------
double* vtkPVImplicitPlaneRepresentation::GetTransformedNormal()
{
  double * norm = this->Superclass::GetNormal();  
  this->InverseTransform->TransformNormal(norm,this->ScaledNormal);
  return this->ScaledNormal;
}

//----------------------------------------------------------------------------
double* vtkPVImplicitPlaneRepresentation::GetTransformedOrigin()
{
  double * origin = this->Superclass::GetOrigin();
  this->InverseTransform->TransformPoint(origin, this->ScaledOrigin);
  return this->ScaledOrigin;
}

//----------------------------------------------------------------------------
void vtkPVImplicitPlaneRepresentation::PlaceTransformedWidget(double bounds[6])
{
  double tempBounds[6];

  double point[3] = {bounds[0], bounds[2], bounds[4] };
  double point2[3] = {bounds[1], bounds[3], bounds[5] };
  //cout << "Point 1 before transform is " << point[0] << ", " << point[1] << ", " << point[2] << endl;
  //cout << "Point 2 before transform is " << point2[0] << ", " << point2[1] << ", " << point2[2] << endl;
  this->Transform->TransformPoint(point, point);
  tempBounds[0] = point[0];
  tempBounds[2] = point[1];
  tempBounds[4] = point[1];

  this->Transform->TransformPoint(point2, point2);
  tempBounds[1] = point2[0];
  tempBounds[3] = point2[1];
  tempBounds[5] = point2[1];
  //cout << "Point 1 after transform is " << point[0] << ", " << point[1] << ", " << point[2] << endl;
  //cout << "Point 2 after transform is " << point2[0] << ", " << point2[1] << ", " << point2[2] << endl;

  this->Superclass::PlaceWidget(tempBounds);
}

//----------------------------------------------------------------------------
void vtkPVImplicitPlaneRepresentation::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
