# Dark Matter Parameter Scanning
This is a package designed to interface with SOFTSUSY or other spectrum generation tool and the micrOMEGAs package for calculating various dark matter properties to provide a variety of ways to scan dark matter parameter space.

## micrOMEGAs
micrOMEGAs is an application using the CalcHEP package to calculate various dark matter properties for a variety of models, most importantly the relic density (omega), scattering cross-sections, and annihilation cross-sections.  Input for a given model is determined by the model files used, but custom output must be coded for a given model if the default output is either insufficient, or too extensive given computational resources.

https://lapth.cnrs.fr/micromegas/

## This Package
This package uses a custom micrOMEGAs input file along with a spectrum generator (SOFTSUSY [http://softsusy.hepforge.org/] for supersymmetric spectrums or another small program using a diagonalization algortihm for simplified models) to produce parameter space scans.  It can be used for a simple scan over input parameters, but also allows for scans fixing output parameters and/or physical quantities through root-solving methods.  In general input parameters cannot be solved for analytically in terms of output quantities.

Several root-solving methodologies are employed for this purpose, given the complexity of dependence of output quantities on input parameters.  While some physical quantities have easily predictable dependence on input parameters, others have unpredictable dependence and the possibility of multiple roots that must be controlled for by boundaries on the root solving routine in the scan methodology.  Numerical precision from SOFTSUSY in particular can also become problematic, introducing an unpredicatable offset to physical quantities which is typically appropriate but difficult to control.

## Requirements
This package requires the presence of a spectrum generator or micrOMEGAs with an appropriate model loaded, and both are generally expected.
