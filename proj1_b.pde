
// A Table object for storing heartrate data
Table table;
// A PImage object for each snapshot image and the concert logo at the bottom
PImage img1, img2, img3, img4, img5;
// A PFont object for our chosen font, Courier new
PFont courier;

// Declarations and imports necessary for using Serial communication with the Arduino
import processing.serial.*;
Serial myPort;
String serialValue;
float val;

float[] footX = new float[0]; // We store the time values for each footstep as we receive them from the Arduino
float[] footY = new float[0]; // We store the random y-coordinates for each footstep after we generate them
float[] footR = new float[0]; // We store the random radii for each footstep after we generate them

void setup() {
  // Initialize serial port; Arduino serial output should also be set to 9600 baud
  myPort = new Serial(this, "COM4", 9600);
  size(766, 400); // Set canvas size
  table = loadTable("sample_heartrate.csv", "header"); // Load the heartrate table
  // Load the concert images
  img1 = loadImage("img1.jpg");  
  img2 = loadImage("img2.png");
  img3 = loadImage("img3.jpg");
  img4 = loadImage("img4.jpg");
  courier = createFont("Courier New", 16); // Initialize font
}

void draw() {
  background(0); //Set background color to black
  
  // Display event details as text
  
  stroke(255); // Set stroke (outline) color to white
  fill(255); // Set fill color to white
  // Settings for text:
  textSize(10);
  textFont(courier);
  textAlign(LEFT);
  text("Simmi", 10, 30);
  textAlign(RIGHT);
  text("Beyonce, Formation Tour    Feb 11, 2019    West Lafayette", width-10, 30);
  
  // Read serial input and plot footsteps
  
  val = 0;
  if (myPort != null && myPort.available() > 0)
  {
    serialValue = myPort.readStringUntil('\n');// Read a line from serial input; should use println in Arduino code
    if(serialValue != null){ // If no Serial input, serialValue will be null
      val = Float.parseFloat(serialValue.trim()); // Convert String to float
      println(val);
      // We append the footstep values to an array so every footprint will be drawn with each loop iteration
      footX = append(footX, val); // Append read value (time of footstep) to array of footstep x-coordinates
      footY = append(footY, (int) random(50,height-100)); // Append random y-coordinate to array of footstep y-coordinates
      // Range of y-coordinate restricted to below header and above most heartrate data.
      footR = append(footR, (int) random(10)); // Append random radius to array of footstep radii
    }
  } else if (myPort == null) {
    println("COM port error");
  }
  fill(50); // Set fill color to white
  // Plot ellipses using array data
  for(int i = 0; i < footX.length; i++) {
    ellipse(footX[i], footY[i], footR[i], footR[i]);
  }
  
  // Plot heartrate data from table
  
  int hr = 0; // hr = heart rate = y axis
  int time = 0; // time = x axis
  stroke(255); // Set draw color to white
  for (TableRow row : table.rows()) { // Iterate through heartrate data
    hr = row.getInt("Heartrate"); // Get heartrate for current row
    point(time++,(height - hr)); // Draw a point for that heartrate sample
  }
  
  // Create "snapshot" points
  // For each of the images, the peak (x,y) is hardcoded
  
  int clickRadius = 10; //radius of hover-able area is 10 pixels
  int imgX = 53;
  int imgY = 90;
  fill(255,0,0); // Set fill color to black
  ellipse(imgX,imgY+img1.height/2,clickRadius,clickRadius);
  
  if(abs(mouseX - imgX) < 10 && abs(mouseY - (imgY+img1.height/2)) < 10) {
      // Displays the image at point (0, height/2) at half of its size
      image(img1, imgX, imgY, img1.width/2, img1.height/2);
  }
  
  // The same steps are repeated for the remaining images:
  
  imgX = 155;
  imgY = 120;
  ellipse(imgX,imgY+img2.height/4,clickRadius,clickRadius);
  
  if(abs(mouseX - imgX) < 10 && abs(mouseY - (imgY+img2.height/4)) < 10) {
      // Displays the image at point (0, height/2) at half of its size
      image(img2, imgX, imgY, img2.width/4, img2.height/4);
  }
  
  imgX = 285;
  imgY = 140;
  ellipse(imgX,imgY+img3.height/6,clickRadius,clickRadius);
  

  if(abs(mouseX - imgX) < 10 && abs(mouseY - (imgY+img3.height/6)) < 10) {
      // Displays the image at point (0, height/2) at half of its size
      image(img3, imgX, imgY, img3.width/6, img3.height/6);
  }
  
  imgX = 635;
  imgY = 100;
  ellipse(imgX,imgY+img4.height/6,clickRadius,clickRadius);
  
  if(abs(mouseX - imgX) < 10 && abs(mouseY - (imgY+img4.height/6)) < 10) {
      // Displays the image at point (0, height/4) at half of its size
      image(img4, imgX-img4.width/6, imgY, img4.width/6, img4.height/6);
  }
  
  //imgX = 735;
  //imgY = 160;
  //ellipse(imgX,imgY+img1.height/2,clickRadius,clickRadius);
  
  //if(abs(mouseX - imgX) < 10 && abs(mouseY - (imgY+img1.height/2)) < 10) {
  //    // Displays the image at point (0, height/2) at half of its size
  //    image(img1, imgX-img1.width/2, imgY, img1.width/2, img1.height/2);
  //}
}
