Week 4:

  Milestones- This wekk I established the overall structure of the project and began
  the constructions of the core logic required for fuzzers. I created essential directories,
  such as targets, crashes and reports to keep a track of important information
  in their respective folder. I also wrote th einitial version of my fuzzer by implmenting
  the alogirthms such as bit_flip, byte_file, arithemetic, dictionary, havoc and normal. I also
  established target functions, with intenttional vulnerabilities to simulate a real world fuzzer.
  By the end of this week, I had a working base for the project, where my fuzzer could read input, mutate it
  and test it against my targets.

  Learnings- This week, I solidified my understanding of how fuzzers work at low level. I learned how
  simple mutations such as bit & byte flips can impact the inputs by corrupting the outputs, and impacting
  the functions they are fed in. Building the initial fuzzer also provided insights about the I/O process
  handaling in C, with processes such as (fork, exec, waitpid). Reading about current fuzzers, also helped
  me to understand my project better, hence making it a very lesson filled journey.

  Challenges- The biggest challenge that I faced was to design a flexible structure that allowed for multiple
  mutation stratergies to be used. This was done by using switch cases. The helped me manage buffers safely,
  especially the risks of null terminators or out of bounds errors. I also faced initial difficulties in ensuring
  that the test targets only crashed due to the input and not randomly. For this I had to carefully go through 
  the logic of fuzzer.c to ensure the inputs to the fucntions were the correct ones. These difficulties, helped my to
  appriciate, real fuzzers and also highlighted how fragile real world programs are to vulnerable inputs.

  Readings- 
  1. What is Fuzz Testing- https://www.blackduck.com/glossary/what-is-fuzz-testing.html
  2. What is AFL++- https://courses.cs.vt.edu/cs3214/spring2022/sfi/concepts_afl
  3. File Processes in C- https://www.geeksforgeeks.org/c/c-file-io/


Week 5:

  Milestones- This week, I focused on enhancing my crash detection and reporting for my fuzzer. I utilised the
  AddressSanitizer (ASan) functionalities to track all the target function and extent the run_target function to
  capture and log the stderr output. My goals was to create detailed crash reports, containing data such as signal, mutation strategry used, when it was done and the ASan error. To add the ASan error into my report, I had to make a new folder called errors, which stored the ASan errors in plain text. I then copied that plain text into crash reports. I also begin developing th HTML based crash report viewer. I did this by creating a html_report.c file, which displayed the crash report in HTML style.

  Learnings- Through the integration of ASan, I learnt how memory corruption issues such as buffer overflowes, heap use after free, heap overflow and format strings are detected at runtime. I studied how the ASan logs, can be used to classify the error type, so that it can be stored in the report. On the front end size, I learnt new skills regarding HTML CSS, to create a presentable front end. This taught me the improtance of presentation, especially when it is something like a report, which would need to be upto industry standard.

  Challenges- Parsing the ASan output was quite tricky due to the different formating of the error outputs. It took me a long time and multple tries, to write a parser, which was able to get the error type from the ASan error. Another challenge was to print valid HTML from the C file, due to the nature of HTMl and its special characters and indentation. I also found linking the ASan error to the crash input a bit diffucult, but it was solved by algorithm manipulations, and use of helper functions.

  Readings- 
  1. Address Sanitizer Guide- https://www.osc.edu/resources/getting_started/howto/howto_use_address_sanitizer
  2. Formatting C code into HTML- https://www.codedrome.com/formatting-c-code-as-html/
  3. Best way to generate HTML from C- https://softwareengineering.stackexchange.com/questions/170258/what-is-the-best-way-to-create-html-in-c-code
  4. Fuzzing and its bugs- https://owasp.org/www-community/Fuzzing

Week 6:

  Milestones- This week I worked on the visual and analysitcal aspects of my report. I added a radar chart to my report using the Chart.js to visually represent the errors based on exploitablity, detection difficulty and complexity. I replaced the raw time stamps with a better formatted version of date and time by using the strftime. I also worked on the formatting of the report, to make it more presentable and professional. These improvements turned my tool from a basic fuzzer into a tool that replicates the functionalities of the real security world through an professional report.

  Learnings- I gained a deep understanding of vulnerbaility assesments by visually mapping out the error severity. This helped me simulate how real security engineers would prioritise fixing bugs. Working with features from Chart.js, helped me to learn about visual graph representation in reports thourgh HTML and C. I also learnt how visual data can improve the quality of security reports. On the backend, I furhter worked on reinforcing the C code by separating logic into helper functions, ensuring future maintainability and expansion of the system.

  Challenges- One of the hardest parts was to design scores for errors. It was easy to exaggerate bugs, and hence I refered to professional documents, with helped me to score the errors properly. Another challenge, was to work on the small bits and pieces in making my report look professional. There were a few issues with sizing of the charts and text boxes, which I researched and worked upon. I utilised open source sites to help me make my report more professional.

  Readings- 
  1. CSS - https://www.w3schools.com/css/
  2. Radar Charts Examples- https://www.smashingmagazine.com/2024/02/draw-radar-charts-web/
  3. Cyber Secuirty Reports (Referencing for my design)- https://www.crowdstrike.com/en-us/global-threat-report/ , https://cybercx.com.au/news/cybercx-2025-threat-report-media-release/

Week 7:

  Milestones- 

  Learnings- 

  Challenges- 

  Readings-

Week 8:

  Milestones- 

  Learnings- 

  Challenges- 

  Readings-
