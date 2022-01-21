<?php
  $streaming = !(isset($_GET['static']));
  $disposition = isset($_GET['inline']) ? "inline" : "attachment";

  $path = "/dev/pdfWriter/";
  $appName = "pdfWriter";
  $jsonFileName = "testdata.json";
  
  $fileName = "Noordshow-M-Apperlo-versie-1.pdf";
  $outputFolder = "output/";

  header( 'Content-Type: application/pdf' );
  header( 'Content-Disposition: ' . $disposition . '; filename="' . $fileName . '"' );

  if ( $streaming ) {

    $descriptorspec = array(
      0 => array("pipe", "r"), 
      1 => array("pipe", "w"), 
      2 => array("file", "/tmp/aopdf-error-output.txt", "a")
    );
    
    $process = proc_open( $path . $appName, $descriptorspec, $pipes, $path );
    
    if (is_resource($process)) {    
      fwrite( $pipes[0], str_replace("\n", "", file_get_contents( $path . $jsonFileName )) );
      fclose($pipes[0]);
  
      while (!feof($pipes[1])) {
        echo fgets($pipes[1], 1024);
      }
      fclose($pipes[1]);

      proc_close($process);
    }

  } else {
    $fullFileName = $path . $outputFolder . $fileName;
    header( 'Content-Length: ' . filesize( $fullFileName ) );
    shell_exec ( "cd " . $path . " && ./" . $appName . " " . $jsonFileName );
    readfile( $fullFileName );
    shell_exec ( 'rm ' . $path . $outputFolder . $fileName );
  }
