# Python test file for multi-language parsing validation

from typing import List, Dict, Optional, Generic, TypeVar
import asyncio
import dataclasses

T = TypeVar('T')

@dataclasses.dataclass
class TestDataClass:
    name: str
    value: int
    metadata: Optional[Dict[str, str]] = None
    
    def __post_init__(self):
        if self.metadata is None:
            self.metadata = {}

class GenericTestClass(Generic[T]):
    def __init__(self, items: List[T]):
        self._items = items
    
    def add_item(self, item: T) -> None:
        self._items.append(item)
    
    def get_items(self) -> List[T]:
        return self._items.copy()
    
    def process_items(self, func) -> List[T]:
        return [func(item) for item in self._items]

async def async_test_function(delay: float) -> str:
    await asyncio.sleep(delay)
    return f"Completed after {delay} seconds"

def decorator_example(func):
    def wrapper(*args, **kwargs):
        print(f"Calling {func.__name__}")
        result = func(*args, **kwargs)
        print(f"Finished {func.__name__}")
        return result
    return wrapper

@decorator_example
def sample_function(x: int, y: int) -> int:
    """Sample function with type hints and decorator."""
    return x + y

class ContextManager:
    def __enter__(self):
        print("Entering context")
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        print("Exiting context")
        return False

def main():
    # Test data class
    test_data = TestDataClass("test", 42, {"key": "value"})
    print(f"Data: {test_data}")
    
    # Test generic class
    generic_test = GenericTestClass([1, 2, 3, 4, 5])
    generic_test.add_item(6)
    doubled = generic_test.process_items(lambda x: x * 2)
    print(f"Doubled: {doubled}")
    
    # Test function
    result = sample_function(10, 20)
    print(f"Function result: {result}")
    
    # Test context manager
    with ContextManager() as cm:
        print("Inside context")
    
    # Test list comprehension and lambda
    numbers = [1, 2, 3, 4, 5]
    squared = [x**2 for x in numbers if x % 2 == 0]
    print(f"Squared evens: {squared}")
    
    # Test async (would need to be called differently in real code)
    # asyncio.run(async_test_function(0.1))

if __name__ == "__main__":
    main()
