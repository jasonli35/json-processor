# WI-24 Project Overview - JSON Processor

## Objective
Develop a comprehensive JSON processing system. This involves:

Parsing JSON text files.
Constructing an in-memory representation of the parsed JSON.
Designing a query language to interact with the in-memory model and extract information.

## Key Components
### Understanding JSON
JSON (JavaScript Object Notation) is a lightweight data-interchange format that is easy for humans to read and write, and easy for machines to parse and generate. It is used extensively in web development and other programming environments. JSON structures include objects ({}) and arrays ([]), capable of containing a variety of data types (strings, numbers, booleans, etc.).

### The Parser
We provide a fully-functional JSON parser. Your task is to integrate this parser with your code by extending the JSONListener class. Implement the listener methods to construct your in-memory model whenever JSON elements are parsed.

### The In-Memory Model
Construct a model to represent JSON data in memory, enhancing data access and manipulation efficiency. This model should be a graph-like structure to accommodate the nested and dynamic nature of JSON data. Suitable STL containers (like std::map for objects and std::vector for lists) should be used to store the data efficiently.

### Query Language
Create a ModelQuery class to navigate and manipulate the in-memory model. Implement functions to:

Traverse the model using a custom query syntax.
Filter and select data based on specific criteria.
Retrieve and compute data such as counting nodes or summing values.
### Project Implementation Details
Parsing
Familiarize yourself with the provided JSON parser.
Implement listener methods to populate your in-memory model.
Building the Model
Decide on appropriate data structures.
Manage various JSON data types and structures effectively.
Query Command System
Develop commands to navigate and manipulate the model.
Ensure robust error handling and validation.
