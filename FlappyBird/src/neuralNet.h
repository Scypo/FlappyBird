#pragma once
#include<vector>
#include<random>

struct Neuron
{
	std::vector<float> weights;
	float bias = 0.0f;

};
class NeuralNet
{
public:
	NeuralNet() = default;
	NeuralNet(size_t inputSize, const std::vector<size_t>& layerSize)
	{
		//set correct sizes
		size_t lcount = layerSize.size();
		layers.resize(lcount);
		for (size_t i = 0; i < lcount; i++)
		{
			layers[i].resize(layerSize[i]);
		}

		//set random values
		std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

		size_t inputs = inputSize;
		for (std::vector<Neuron>& layer : layers)
		{
			for (Neuron& neuron : layer)
			{
				neuron.weights.resize(inputs);
				for (float& w : neuron.weights)
				{
					w = dist(rng);
				}
				neuron.bias = dist(rng);
			}
			inputs = layer.size();
		}
	}
	std::vector<float> Evaluate(std::vector<float> input) const
	{
		std::vector<float> current;

		for (const std::vector<Neuron>& layer : layers)
		{
			current.resize(layer.size());
			for (size_t i = 0; i < layer.size(); i++)
			{
				current[i] = EvaluateNeuron(layer[i], input);
			}
			std::swap(current, input);
		}
		return input;
	}
	const std::vector<std::vector<Neuron>>& GetLayers() const
	{
		return layers;
	}
	static NeuralNet GetChildNet(const NeuralNet& n1, const NeuralNet& n2)
	{
		//resize and assume both parents have same structure
		NeuralNet child;
		child.layers = n1.layers;

		//init random
		std::uniform_real_distribution<float> mutDist(-0.1f, 0.1f);
		std::uniform_real_distribution<float> chance(0.0f, 1.0f);

		for (size_t i = 0; i < n1.layers.size(); i++)
		{
			for (size_t j = 0; j < n1.layers[i].size(); j++)
			{
				//inherit bias
				float biasAncestor = chance(rng);
				if (biasAncestor < 0.2f)
				{
					child.layers[i][j].bias = (n1.layers[i][j].bias + n2.layers[i][j].bias) * 0.5f;
				}
				else if (biasAncestor < 0.6f)
				{
					child.layers[i][j].bias = n1.layers[i][j].bias;
				}
				else if (biasAncestor <= 1.0f)
				{
					child.layers[i][j].bias = n2.layers[i][j].bias;
				}
				//mutate bias
				if (chance(rng) < 0.1f)
				{
					child.layers[i][j].bias += mutDist(rng);
				}

				for (size_t k = 0; k < n1.layers[i][j].weights.size(); k++)
				{
					//inherit neuron weight
					float ancestorChance = chance(rng);
					if(ancestorChance < 0.2f)
					{
						child.layers[i][j].weights[k] = (n1.layers[i][j].weights[k] + n2.layers[i][j].weights[k]) * 0.5f;
					}
					else if (ancestorChance < 0.6f)
					{
						child.layers[i][j].weights[k] = n1.layers[i][j].weights[k];
					}
					else if(ancestorChance <= 1.0f)
					{
						child.layers[i][j].weights[k] = n2.layers[i][j].weights[k];
					}
					//mutate neuron
					if (chance(rng) < 0.1f)
					{
						child.layers[i][j].weights[k] += mutDist(rng);
					}
				}
			}
		}

		return child;
	}
private:
	float EvaluateNeuron(const Neuron& n, const std::vector<float>& input) const
	{
		assert(input.size() == n.weights.size());
		float sum = n.bias;
		for (size_t i = 0; i < n.weights.size(); i++)
		{
			sum += n.weights[i] * input[i];
		}
		return Activate(sum);
	}
	float Activate(float sum) const
	{
		return std::tanh(sum);
	}
private:
	std::vector<std::vector<Neuron>> layers;
	static inline std::mt19937 rng{ std::random_device{}() };
};