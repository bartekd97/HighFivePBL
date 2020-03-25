using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class SliderValueLabel : MonoBehaviour
{
    public Slider slider;

    void Update()
    {
        GetComponent<Text>().text = slider.value.ToString();
    }
}
