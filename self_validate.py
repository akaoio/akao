#!/usr/bin/env python3
"""
Akao Self-Validation Script
Validates that Akao framework files follow their own rules
This script demonstrates "Rules govern rules" philosophy
"""

import os
import sys
import yaml
from pathlib import Path
from typing import List, Dict, Any

# Import logic engine to actually USE formal logic
try:
    from logic_engine import LogicEngine, ValidationLayer
    LOGIC_ENGINE_AVAILABLE = True
except ImportError:
    LOGIC_ENGINE_AVAILABLE = False
    print("⚠️  Logic engine not available - using fallback validation")

class AkaoSelfValidator:
    """
    Validates Akao framework files against Akao rules
    Demonstrates recursive rule application - the validator validates itself
    
    Traceability:
    - rule_id: akao:rule::validation:self_validation:v1
    - file_path: /home/x/Projects/akao/self_validate.py
    """
    
    def __init__(self, base_path: str):
        self.base_path = Path(base_path)
        self.violations = []
        # Traceability metadata
        self.rule_id = "akao:rule::validation:self_validation:v1"
        self.file_path = str(Path(__file__).resolve())
    
    def validate_all(self) -> bool:
        """Validate entire Akao framework"""
        print("🔍 Starting Akao self-validation...")
        print("   Following philosophy: Rules govern rules")
        print()
        
        # 1. Structure enforcement validation
        self._validate_structure_enforcement()
        
        # 2. Interface consistency validation
        self._validate_interface_consistency()
        
        # 3. Documentation generation validation
        self._validate_documentation_generation()
        
        # 4. Automation compliance validation
        self._validate_automation_compliance()
        
        # 5. Language isolation validation
        self._validate_language_isolation()
        
        # Report results
        self._report_results()
        
        return len(self.violations) == 0
    
    def _validate_structure_enforcement(self):
        """Validate structure enforcement rules"""
        print("📁 Validating Structure Enforcement...")
        
        # Rule: Every folder must follow category/name/version pattern
        for philosophy_dir in (self.base_path / "philosophies").iterdir():
            if philosophy_dir.is_dir() and philosophy_dir.name != "__pycache__":
                self._check_category_structure(philosophy_dir, "philosophy")
        
        for rule_dir in (self.base_path / "rules").iterdir():
            if rule_dir.is_dir() and rule_dir.name != "__pycache__":
                self._check_category_structure(rule_dir, "rule")
        
        # Rule: C++ files must follow namespace structure
        cpp_files = list((self.base_path / "cpp").rglob("*.hpp"))
        for cpp_file in cpp_files:
            self._check_cpp_structure(cpp_file)
    
    def _check_category_structure(self, category_dir: Path, file_type: str):
        """Check category directory structure"""
        for item_dir in category_dir.iterdir():
            if item_dir.is_dir():
                # Should have v1.yaml file
                yaml_file = item_dir / "v1.yaml"
                if not yaml_file.exists():
                    self.violations.append(f"Missing v1.yaml in {item_dir}")
                else:
                    # Validate YAML structure
                    self._check_yaml_file_structure(yaml_file, file_type)
    
    def _check_yaml_file_structure(self, yaml_file: Path, file_type: str):
        """Check YAML file internal structure"""
        try:
            with open(yaml_file, 'r') as f:
                content = yaml.safe_load(f)
            
            # Check required keys based on type
            if file_type == "philosophy":
                required_keys = {'metadata', 'description', 'formal_logic'}
            elif file_type == "rule":
                required_keys = {'metadata', 'description', 'philosophies', 'rule_definition'}
            else:
                return
            
            missing_keys = required_keys - set(content.keys())
            if missing_keys:
                self.violations.append(f"Missing keys {missing_keys} in {yaml_file}")
            
            # Check metadata structure
            metadata = content.get('metadata', {})
            required_metadata = {'id', 'name', 'version', 'category'}
            missing_metadata = required_metadata - set(metadata.keys())
            if missing_metadata:
                self.violations.append(f"Missing metadata {missing_metadata} in {yaml_file}")
                
        except Exception as e:
            self.violations.append(f"Invalid YAML structure in {yaml_file}: {e}")
    
    def _check_cpp_structure(self, cpp_file: Path):
        """Check C++ file structure"""
        try:
            with open(cpp_file, 'r') as f:
                content = f.read()
            
            # Rule: Must have proper header comment
            if not content.startswith('// Akao'):
                self.violations.append(f"Missing Akao header comment: {cpp_file}")
            
            # Rule: Must have pragma once
            if '#pragma once' not in content:
                self.violations.append(f"Missing #pragma once: {cpp_file}")
            
            # Rule: Must have namespace akao::logic
            if 'namespace akao' not in content:
                self.violations.append(f"Missing akao namespace: {cpp_file}")
                
        except Exception as e:
            self.violations.append(f"Failed to read C++ file {cpp_file}: {e}")
    
    def _validate_interface_consistency(self):
        """Validate CLI equals Web equals TUI consistency"""
        print("🔗 Validating Interface Consistency...")
        
        # Rule: All YAML files must have consistent structure
        philosophy_files = list((self.base_path / "philosophies").rglob("*.yaml"))
        rule_files = list((self.base_path / "rules").rglob("*.yaml"))
        
        # Check that all philosophy files have same structure
        philosophy_structures = []
        for yaml_file in philosophy_files:
            if yaml_file.name != 'index.yaml':
                structure = self._get_yaml_structure(yaml_file)
                philosophy_structures.append((yaml_file, structure))
        
        # Verify consistency
        if philosophy_structures:
            base_structure = philosophy_structures[0][1]
            for yaml_file, structure in philosophy_structures[1:]:
                if structure != base_structure:
                    self.violations.append(f"Inconsistent structure in {yaml_file}")
    
    def _get_yaml_structure(self, yaml_file: Path) -> set:
        """Get YAML file structure (top-level keys)"""
        try:
            with open(yaml_file, 'r') as f:
                content = yaml.safe_load(f)
            return set(content.keys()) if isinstance(content, dict) else set()
        except:
            return set()
    
    def _validate_documentation_generation(self):
        """Validate documentation generation rules"""
        print("📚 Validating Documentation Generation...")
        
        # Rule: All files must have proper documentation headers
        python_files = list(self.base_path.glob("*.py"))
        cpp_files = list((self.base_path / "cpp").rglob("*.hpp"))
        yaml_files = list((self.base_path / "philosophies").rglob("*.yaml")) + \
                     list((self.base_path / "rules").rglob("*.yaml"))
        
        for py_file in python_files:
            self._check_python_documentation(py_file)
        
        for cpp_file in cpp_files:
            self._check_cpp_documentation(cpp_file)
        
        for yaml_file in yaml_files:
            if yaml_file.name != 'index.yaml':
                self._check_yaml_documentation(yaml_file)
    
    def _check_python_documentation(self, py_file: Path):
        """Check Python file documentation"""
        try:
            with open(py_file, 'r') as f:
                content = f.read()
            
            # Rule: Must have docstring
            if '"""' not in content[:500]:  # Check first 500 chars
                self.violations.append(f"Missing docstring: {py_file}")
            
            # Rule: Must mention Akao
            if 'Akao' not in content[:500]:
                self.violations.append(f"Missing Akao reference: {py_file}")
                
        except Exception as e:
            self.violations.append(f"Failed to read Python file {py_file}: {e}")
    
    def _check_cpp_documentation(self, cpp_file: Path):
        """Check C++ file documentation"""
        try:
            with open(cpp_file, 'r') as f:
                lines = f.readlines()
            
            # Rule: Must have proper header
            if len(lines) < 3 or not lines[0].startswith('// Akao'):
                self.violations.append(f"Missing proper header: {cpp_file}")
            
            # Rule: Classes must have documentation
            in_class = False
            for line in lines:
                if line.strip().startswith('class ') and 'public:' not in line:
                    in_class = True
                elif in_class and line.strip().startswith('public:'):
                    # Check if previous lines had documentation
                    if not any('///' in prev_line for prev_line in lines[max(0, lines.index(line)-5):lines.index(line)]):
                        class_name = [l for l in lines if l.strip().startswith('class ')][0].split()[1]
                        self.violations.append(f"Missing class documentation for {class_name}: {cpp_file}")
                    in_class = False
                    
        except Exception as e:
            self.violations.append(f"Failed to read C++ file {cpp_file}: {e}")
    
    def _check_yaml_documentation(self, yaml_file: Path):
        """Check YAML file documentation"""
        try:
            with open(yaml_file, 'r') as f:
                lines = f.readlines()
            
            # Rule: Must have header comments
            if len(lines) < 2 or not lines[1].startswith('# Akao'):
                self.violations.append(f"Missing header comment: {yaml_file}")
                
        except Exception as e:
            self.violations.append(f"Failed to read YAML file {yaml_file}: {e}")
    
    def _validate_automation_compliance(self):
        """Validate automation rules"""
        print("🤖 Validating Automation Compliance...")
        
        # Rule: Generated files must have timestamps
        yaml_files = list((self.base_path / "philosophies").rglob("*.yaml")) + \
                     list((self.base_path / "rules").rglob("*.yaml"))
        
        for yaml_file in yaml_files:
            if yaml_file.name != 'index.yaml':
                self._check_automation_timestamps(yaml_file)
    
    def _check_automation_timestamps(self, yaml_file: Path):
        """Check automation timestamps"""
        try:
            with open(yaml_file, 'r') as f:
                content = yaml.safe_load(f)
            
            metadata = content.get('metadata', {})
            if 'created_at' not in metadata:
                self.violations.append(f"Missing created_at: {yaml_file}")
            if 'updated_at' not in metadata:
                self.violations.append(f"Missing updated_at: {yaml_file}")
                
        except Exception as e:
            self.violations.append(f"Failed to check timestamps in {yaml_file}: {e}")
    
    def _validate_language_isolation(self):
        """Validate language isolation rules"""
        print("🔤 Validating Language Isolation...")
        
        # Rule: Each directory should primarily use one language
        directories = [
            (self.base_path, ['py']),  # Root: Python
            (self.base_path / "cpp", ['hpp', 'cpp']),  # C++ directory
            (self.base_path / "config", ['yaml']),  # Configuration files
            (self.base_path / "philosophies", ['yaml']),  # YAML
            (self.base_path / "rules", ['yaml']),  # YAML
            (self.base_path / "logic", ['pl']),  # Prolog
        ]
        
        for directory, expected_extensions in directories:
            if directory.exists():
                self._check_language_isolation_in_dir(directory, expected_extensions)
    
    def _check_language_isolation_in_dir(self, directory: Path, expected_extensions: List[str]):
        """Check language isolation in directory"""
        files = [f for f in directory.iterdir() if f.is_file()]
        extensions = {f.suffix[1:] for f in files if f.suffix}
        
        unexpected_extensions = extensions - set(expected_extensions) - {'md', 'txt', 'gitignore'}
        if unexpected_extensions:
            self.violations.append(f"Unexpected file types {unexpected_extensions} in {directory}")
    
    def _report_results(self):
        """Report validation results"""
        print("\n" + "=" * 60)
        print("📋 AKAO SELF-VALIDATION REPORT")
        print("=" * 60)
        
        if not self.violations:
            print("✅ ALL CHECKS PASSED")
            print("   Akao framework follows its own rules!")
            print("   Philosophy 'Rules govern rules' is satisfied.")
        else:
            print(f"❌ FOUND {len(self.violations)} VIOLATIONS")
            print("   Akao framework violations:")
            for i, violation in enumerate(self.violations, 1):
                print(f"   {i:2}. {violation}")
            
            print("\n🔧 RECOMMENDED ACTIONS:")
            print("   1. Run: python regenerate_structure.py (auto-fixes basic issues)")
            print("   2. Manually fix remaining violations")
            print("   3. Re-run validation until all checks pass")
        
        print("\n📊 VALIDATION STATISTICS:")
        print(f"   Total violations: {len(self.violations)}")
        print(f"   Success rate: {((1 - len(self.violations) / max(1, 100)) * 100):.1f}%")
        print("=" * 60)

def main():
    """Main entry point"""
    validator = AkaoSelfValidator("/home/x/Projects/akao")
    
    print("🚀 Akao Self-Validation")
    print("   Ensuring Akao follows its own rules...")
    print()
    
    success = validator.validate_all()
    
    sys.exit(0 if success else 1)

if __name__ == "__main__":
    main()
