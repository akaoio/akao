// JavaScript test file for multi-language parsing validation

// ES6+ features test
class TestClass {
    constructor(name, value) {
        this.name = name;
        this.value = value;
        this._data = new Map();
    }
    
    // Getter and setter
    get displayName() {
        return `${this.name}: ${this.value}`;
    }
    
    set updateValue(newValue) {
        this.value = newValue;
    }
    
    // Async method
    async fetchData(url) {
        try {
            const response = await fetch(url);
            const data = await response.json();
            return data;
        } catch (error) {
            console.error('Fetch error:', error);
            throw error;
        }
    }
    
    // Generator method
    *generateNumbers(max) {
        for (let i = 0; i < max; i++) {
            yield i * 2;
        }
    }
    
    // Arrow functions and destructuring
    processArray = (arr) => {
        return arr
            .filter(x => x > 0)
            .map(x => x * 2)
            .reduce((acc, val) => acc + val, 0);
    }
}

// Module pattern
const TestModule = (function() {
    let privateVar = 0;
    
    function privateFunction() {
        return privateVar++;
    }
    
    return {
        increment: () => privateFunction(),
        getCount: () => privateVar,
        
        // Object destructuring and spread
        mergeObjects: (obj1, obj2) => ({...obj1, ...obj2}),
        
        // Template literals
        formatMessage: (name, age) => `Hello ${name}, you are ${age} years old`
    };
})();

// Promises and async/await
function createPromise(value, delay) {
    return new Promise((resolve, reject) => {
        setTimeout(() => {
            if (value > 0) {
                resolve(value * 2);
            } else {
                reject(new Error('Value must be positive'));
            }
        }, delay);
    });
}

// Destructuring assignment
function processUser({name, age, email = 'not provided'}) {
    return {
        displayName: name.toUpperCase(),
        isAdult: age >= 18,
        contact: email
    };
}

// Higher-order functions
const functionalExample = {
    numbers: [1, 2, 3, 4, 5],
    
    // Method using arrow function
    processNumbers: function() {
        return this.numbers
            .map(n => n ** 2)
            .filter(n => n % 2 === 0)
            .sort((a, b) => b - a);
    },
    
    // Currying example
    multiply: (a) => (b) => a * b,
    
    // Function composition
    compose: (f, g) => (x) => f(g(x))
};

// Main execution
function main() {
    // Test class
    const test = new TestClass('Test', 42);
    console.log(test.displayName);
    
    // Test generator
    const generator = test.generateNumbers(5);
    for (const num of generator) {
        console.log('Generated:', num);
    }
    
    // Test array processing
    const result = test.processArray([1, -2, 3, -4, 5]);
    console.log('Processed array result:', result);
    
    // Test module
    console.log('Count:', TestModule.getCount());
    TestModule.increment();
    console.log('After increment:', TestModule.getCount());
    
    // Test destructuring
    const user = {name: 'John', age: 25};
    const processed = processUser(user);
    console.log('Processed user:', processed);
    
    // Test functional programming
    const doubled = functionalExample.multiply(2);
    console.log('Doubled 5:', doubled(5));
    
    const processed_numbers = functionalExample.processNumbers();
    console.log('Processed numbers:', processed_numbers);
    
    // Test promises (commented out to avoid actual execution)
    /*
    createPromise(5, 100)
        .then(result => console.log('Promise result:', result))
        .catch(error => console.error('Promise error:', error));
    */
}

// Export for module systems
if (typeof module !== 'undefined' && module.exports) {
    module.exports = {
        TestClass,
        TestModule,
        createPromise,
        processUser,
        functionalExample
    };
}

// Run main if in browser or direct execution
if (typeof window !== 'undefined' || require.main === module) {
    main();
}
