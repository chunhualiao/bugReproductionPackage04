#include <rose.h>
#include <stdio.h>

#include <Outliner.hh>

using namespace SageInterface;

// Wrapper around Rose's Outliner to emulate statelessness
class OutlWrap {
  bool dumpOutlinedCodeIntoNewFile = false;
  bool enableClassicOutliner = true;

public:
  OutlWrap(bool newFile, bool enableClassic)
      : dumpOutlinedCodeIntoNewFile(newFile),
        enableClassicOutliner(enableClassic) {}

  Outliner::Result outline(SgStatement *s) const {
    Outliner::enable_classic = enableClassicOutliner;
    Outliner::useNewFile = dumpOutlinedCodeIntoNewFile;
    // Outliner::naturalScopeHandling = true;
    return Outliner::outline(s);
  }
};

template <typename T> class PostOrderSequencer {
private:
  class Traverser : public AstSimpleProcessing {
  public:
    std::vector<T *> sequence{};
    virtual void atTraversalEnd() {}
    virtual void visit(SgNode *node) {
      auto decl = dynamic_cast<T *>(node);
      if (!decl) {
        return;
      }
      sequence.push_back(decl);
    }
  };

public:
  static std::vector<T *> sequence(SgNode *ast) {
    Traverser traverser{};
    traverser.traverse(ast, postorder);
    return traverser.sequence;
  }
};

template <typename F>
void runTransformationTested(F transformation, const std::string msg,
                             SgProject *project) {
  std::cout << "run ast test before transformation " << msg << std::endl;
  AstTests::runAllTests(project);
  transformation();
  std::cout << "run ast test after transformation " << msg << std::endl;
  AstTests::runAllTests(project);
}

int main(int argc, char **argv) {
  SgProject *project = frontend(argc, argv);

  // query the omp target node ...
  std::vector<SgOmpTargetDataStatement *> targetRegions =
      PostOrderSequencer<SgOmpTargetDataStatement>::sequence(project);
  ROSE_ASSERT(targetRegions.size() == 1);
  SgOmpTargetDataStatement *targetRegion = targetRegions[0];

  ROSE_ASSERT(targetRegion != NULL);

  SgFunctionDeclaration *main = SageInterface::findMain(project);
  SgSourceFile *mainFile = SageInterface::getEnclosingSourceFile(main);

  ROSE_ASSERT(main != NULL);

  // ... get the SgBasicBlock beneath the omp target node ...
  SgStatement *body = targetRegion->get_body();
  ROSE_ASSERT(body->variantT() == V_SgBasicBlock);

  // ... outline the SgBasicBlock ...
  SgFunctionDeclaration *outlinedFunction;
  runTransformationTested(
      [&project, &outlinedFunction, &body]() {
        OutlWrap outliner(false, true);
        auto outlinerResult = outliner.outline(body);
        outlinedFunction = outlinerResult.decl_;
      },
      "[outlining]", project);

  ROSE_ASSERT(outlinedFunction != NULL);

  // ... attempt to clone the new outlined function.
  SgFunctionDeclaration *funcCopy;
  runTransformationTested(
      [&project, &funcCopy, &main, &outlinedFunction]() {
        funcCopy = isSgFunctionDeclaration(copyStatement(outlinedFunction));
        funcCopy->set_name(funcCopy->get_name() + "_COPY");
        // Insert it to a scope
        SgGlobal *glb = getFirstGlobalScope(project);
        insertStatement(main, funcCopy, false, false);
      },
      "[function cloning]", project);
  
  return backend(project);
}
