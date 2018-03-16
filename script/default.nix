{ stdenv, fetchurl }:

stdenv.mkDerivation rec {
  name = "cmdline-${version}";
  version = "1.0";

  src = fetchurl {
    url = "https://github.com/deepsea-inria/cmdline/archive/${version}.tar.gz";
    sha256 = "d947aa8fec769e417684a924e3f5bf76beaacda9c51725d9f955047c98e4ab39";
  };

  dontBuild = true;

  installPhase = ''
    mkdir -p $out/include/
    cp include/cmdline.hpp $out/include/
    mkdir -p $out/doc
    cp README.md Makefile cmdline.css $out/doc/
  '';

  meta = {
    description = "A small, self-contained header file that implements the Deepsea command-line conventions.";
    homepage = http://deepsea.inria.fr/;
  };
}