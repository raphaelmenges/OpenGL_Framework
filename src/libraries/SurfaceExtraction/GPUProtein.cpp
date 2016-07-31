#include "GPUProtein.h"
#include "Molecule/MDtrajLoader/Data/Protein.h"
#include "Molecule/MDtrajLoader/Data/AtomLUT.h"

GPUProtein::GPUProtein(Protein * const pProtein)
{
    // Create structures for CPU
    int atomCount  = pProtein->getAtoms()->size();
    int frameCount = pProtein->getAtomAt(0)->getCountOfFrames(); // TODO: what if no atoms in protein?
    mspRadii = std::shared_ptr<std::vector<float> >(new std::vector<float>);
    mspRadii->reserve(atomCount);

    // Already size trajectory vector
    mspTrajectory = std::shared_ptr<
            std::vector<
                std::vector<glm::vec3> > >(
                    new std::vector<std::vector<glm::vec3> >);
    mspTrajectory->resize(frameCount);
    for(int i = 0; i < frameCount; i++)
    {
        mspTrajectory->at(i).resize(atomCount);
    }

    // Reserve space in other vectors (which are all assumed to be empty)
    mCentersOfMass.reserve(frameCount);
    mElements.reserve(atomCount);
    mAminoacids.reserve(atomCount);

    // Fill radii, elements and aminoacids on CPU
    for(int i = 0; i < atomCount; i++) // go over atoms
    {
        // Collect radius
        mspRadii->push_back(pProtein->getRadiusAt(i));

        // Element
        mElements.push_back(pProtein->getAtomAt(i)->getElement());

        // Aminoacid
        mAminoacids.push_back(pProtein->getAtomAt(i)->getAmino());
    }

    // Fill trajectory on CPU
    for(int i = 0; i < frameCount; i++) // go over frames
    {
        glm::vec3 accPosition(0, 0, 0);
        for(int j = 0; j < atomCount; j++) // go over atoms
        {
            // Position of that atom in that frame
            glm::vec3 position = pProtein->getAtoms()->at(j)->getPositionAtFrame(i);

            // Collect trajectory (already correctly sized)
            mspTrajectory->at(i).at(j) = position;

            // Accumulate position
            accPosition += position;
        }

        // Save center
        mCentersOfMass.push_back(accPosition / atomCount);
    }

    // Init SSBOs
    initSSBOs(atomCount, frameCount);
}

GPUProtein::GPUProtein(const std::vector<glm::vec4>& rAtoms)
{
    // Create structures for CPU
    int atomCount  = rAtoms.size();
    mspRadii = std::shared_ptr<std::vector<float> >(new std::vector<float>);
    mspRadii->resize(atomCount);
    mspTrajectory = std::shared_ptr<
            std::vector<
                std::vector<glm::vec3> > >(
                    new std::vector<std::vector<glm::vec3> >);
    mspTrajectory->resize(1);
    mspTrajectory->at(0).resize(atomCount);

    // Fill structures for CPU
    for(int i = 0; i < atomCount; i++)
    {
        // Collect radius
        mspRadii->at(i) = rAtoms.at(i).w;

        // Collect trajectory
        mspTrajectory->at(0).at(i) = glm::vec3(rAtoms.at(i).x, rAtoms.at(i).y, rAtoms.at(i).z);
    }

    // TODO: Elements and aminoacids are not filled here

    // Init SSBOs
    initSSBOs(atomCount, 0);
}

GPUProtein::~GPUProtein()
{
    glDeleteBuffers(1, &mRadiiSSBO);
    glDeleteBuffers(1, &mTrajectorySSBO);
    glDeleteBuffers(1, &mColorsElementSSBO);
    glDeleteBuffers(1, &mColorsAminoacidSSBO);
}

void GPUProtein::bind(GLuint radiiSlot, GLuint trajectorySlot) const
{
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, radiiSlot, mRadiiSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, trajectorySlot, mTrajectorySSBO);
}

void GPUProtein::bindTrajectory(GLuint slot) const
{
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, slot, mTrajectorySSBO);
}

void GPUProtein::bindColorsElement(GLuint slot) const
{
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, slot, mColorsElementSSBO);
}

void GPUProtein::bindColorsAminoacid(GLuint slot) const
{
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, slot, mColorsAminoacidSSBO);
}

std::shared_ptr<const std::vector<float> > GPUProtein::getRadii() const
{
    return mspRadii;
}

std::shared_ptr<const std::vector<std::vector<glm::vec3> > > GPUProtein::getTrajectory() const
{
    return mspTrajectory;
}

void GPUProtein::initSSBOs(int atomCount, int frameCount)
{
    // For copying it to OpenGL, store it linear
    std::vector<glm::vec3> linearTrajectory;
    linearTrajectory.reserve(frameCount * atomCount);
    for(int i = 0; i < frameCount; i++)
    {
        linearTrajectory.insert(linearTrajectory.end(), mspTrajectory->at(i).begin(), mspTrajectory->at(i).end());
    }

    // Create structures of radii and trajectory on GPU
    glGenBuffers(1, &mRadiiSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, mRadiiSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float) * mspRadii->size(), mspRadii->data(), GL_STATIC_DRAW);
    glGenBuffers(1, &mTrajectorySSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, mTrajectorySSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec3) * linearTrajectory.size(), linearTrajectory.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    // Get atom lookup
    AtomLUT lut;

    // Create structure for coloring according to element on GPU
    std::vector<glm::vec3> elementColors;
    elementColors.reserve(atomCount);
    for(int i = 0; i < atomCount; i++)
    {
        auto color = lut.cpk_colorcode[mElements.at(i)];
        elementColors.push_back(glm::vec3(color.r, color.g, color.b));
    }
    glGenBuffers(1, &mColorsElementSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, mColorsElementSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec3) * elementColors.size(), elementColors.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    // Create structure for coloring according to aminoacid on GPU
    std::vector<glm::vec3> aminoacidColors;
    aminoacidColors.reserve(atomCount);
    for(int i = 0; i < atomCount; i++)
    {
        aminoacidColors.push_back(glm::vec3(1,0,0)); // TODO
    }
    glGenBuffers(1, &mColorsAminoacidSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, mColorsAminoacidSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec3) * aminoacidColors.size(), aminoacidColors.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

