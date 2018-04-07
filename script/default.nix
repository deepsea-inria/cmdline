{ pkgs   ? import <nixpkgs> {},
  stdenv ? pkgs.stdenv,
  cmdlineSrc ? ../.,
  buildDocs ? false
}:

stdenv.mkDerivation rec {
  name = "cmdline";

  src = cmdlineSrc;

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