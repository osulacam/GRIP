package edu.wpi.grip.ui.codegeneration.cv;

import edu.wpi.grip.core.ManualPipelineRunner;
import edu.wpi.grip.core.OperationMetaData;
import edu.wpi.grip.core.Step;
import edu.wpi.grip.core.operations.composite.BlurOperation;
import edu.wpi.grip.core.sockets.InputSocket;
import edu.wpi.grip.core.sockets.OutputSocket;
import edu.wpi.grip.core.sources.ImageFileSource;
import edu.wpi.grip.ui.codegeneration.AbstractGenerationTest;
import edu.wpi.grip.ui.codegeneration.tools.GenType;
import edu.wpi.grip.ui.codegeneration.tools.HelperTools;
import edu.wpi.grip.ui.codegeneration.tools.PipelineInterfacer;
import edu.wpi.grip.util.Files;

import org.junit.Test;
import org.opencv.core.Mat;

import java.util.Optional;

import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

public class CVMax extends AbstractGenerationTest {

  boolean setup() {
    Step blur = gen.addStep(new OperationMetaData(BlurOperation.DESCRIPTION, () -> new
        BlurOperation(isf, osf)));
    ImageFileSource img = loadImage(Files.gompeiJpegFile);
    OutputSocket imgOut = pipeline.getSources().get(0).getOutputSockets().get(0);
    for (InputSocket sock : blur.getInputSockets()) {
      String sockHint = sock.getSocketHint().getIdentifier();
      if (sockHint.equals("Input")) {
        gen.connect(imgOut, sock);
      } else if (sockHint.equals("Radius")) {
        sock.setValue(new Double(10.0));
      } else if (sock.getSocketHint().getIdentifier().equals("Type")) {
        HelperTools.setEnumSocket(sock, "Box Blur");
      }
    }
    Step max = gen.addStep(opUtil.getMetaData("CV Max"));
    gen.connect(imgOut, max.getInputSockets().get(0));
    gen.connect(blur.getOutputSockets().get(0), max.getInputSockets().get(1));
    return true;
  }
  
  @Test
  public void cvMaxTest() {
    test(() -> setup(), (pip) -> validate(pip), "CvMaxTest");
  }
  
  void validate(PipelineInterfacer pip) {
    ManualPipelineRunner runner = new ManualPipelineRunner(eventBus, pipeline);
    runner.runPipeline();
    Optional out = pipeline.getSteps().get(1).getOutputSockets().get(0).getValue();
    assertTrue("Pipeline did not process", out.isPresent());
    assertFalse("Pipeline output is empty", ((org.bytedeco.javacpp.opencv_core.Mat) out.get())
        .empty());
    pip.setMatSource(0, Files.gompeiJpegFile.file);
    pip.process();
    Mat genMat = (Mat) pip.getOutput(1, GenType.IMAGE);
    Mat gripMat = HelperTools.bytedecoMatToCVMat((org.bytedeco.javacpp.opencv_core.Mat) out.get());
    assertMatWithin(genMat, gripMat, 2.0);
  }
}