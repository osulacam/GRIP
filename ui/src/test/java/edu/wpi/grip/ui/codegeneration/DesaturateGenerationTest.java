package edu.wpi.grip.ui.codegeneration;

import org.junit.Test;
import org.opencv.core.Mat;

import java.util.Optional;

import edu.wpi.grip.core.ManualPipelineRunner;
import edu.wpi.grip.core.OperationMetaData;
import edu.wpi.grip.core.Step;
import edu.wpi.grip.core.operations.composite.DesaturateOperation;
import edu.wpi.grip.core.sockets.InputSocket;
import edu.wpi.grip.core.sockets.OutputSocket;
import edu.wpi.grip.core.sources.ImageFileSource;
import edu.wpi.grip.ui.codegeneration.tools.HelperTools;
import edu.wpi.grip.ui.codegeneration.tools.PipelineInterfacer;
import edu.wpi.grip.util.Files;

import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

public class DesaturateGenerationTest extends AbstractGenerationTest {


  void generatePipeline() {
    Step desat = gen.addStep(new OperationMetaData(DesaturateOperation.DESCRIPTION, () -> new DesaturateOperation(isf, osf)));
    ImageFileSource img = loadImage(Files.gompeiJpegFile);
    OutputSocket imgOut = pipeline.getSources().get(0).getOutputSockets().get(0);
    for (InputSocket sock : desat.getInputSockets()) {
      if (sock.getSocketHint().getIdentifier().equals("Input")) {
        gen.connect(imgOut, sock);
      }
    }
  }

  @Test
  public void DesaturationTest() {
    test(() -> {
          generatePipeline();
          return true;
        }, (pip) ->
            testPipeline(pip)
        , "Desat");
  }

  void testPipeline(PipelineInterfacer pip) {
    new ManualPipelineRunner(eventBus, pipeline).runPipeline();
    Optional out = pipeline.getSteps().get(0).getOutputSockets().get(0).getValue();
    assertTrue("Output is not present", out.isPresent());
    assertFalse("Output Mat is empty", ((org.bytedeco.javacpp.opencv_core.Mat) out.get()).empty());
    pip.setMatSource(0, Files.gompeiJpegFile.file);
    pip.process();
    Mat genMat = (Mat) pip.getOutput(0);
    Mat gripMat = HelperTools.bytedecoMatToCVMat((org.bytedeco.javacpp.opencv_core.Mat) out.get());
    assertMatWithin(genMat, gripMat, 10.0);
  }

}
