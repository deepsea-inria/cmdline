{ pkgs   ? import <nixpkgs> {},
  stdenv ? pkgs.stdenv,
  fetchurl,
  buildDocs ? false
}:

stdenv.mkDerivation rec {
  name = "cmdline-${version}";
  version = "v1.0";

  src = fetchurl {
    url = "https://github.com/deepsea-inria/cmdline/archive/${version}.tar.gz";
    sha256 = "1fji3xpadpa70sy7fqcsvc31n0wmkrhv3r0i2a1dgk2iq5mrin3v";
  };

  buildInputs = if buildDocs then [
    pkgs.pandoc pkgs.texlive.combined.scheme-medium
  ] else
    [];

  buildPhase = if buildDocs then ''
    make README.pdf README.html
  ''
  else ''
    # nothing to build
  '';

  installPhase = ''
    mkdir -p $out/include/
    cp include/cmdline.hpp $out/include/
    mkdir -p $out/doc
    cp README.* Makefile cmdline.css $out/doc/
  '';

  meta = {
    description = "A small, self-contained header file that implements the Deepsea command-line conventions.";
    license = "MIT";
    homepage = http://deepsea.inria.fr/;
  };
}