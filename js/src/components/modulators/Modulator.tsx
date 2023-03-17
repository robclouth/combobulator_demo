import { Box, Card, CloseButton, Paper, Stack, Text } from "@mantine/core";
import { useHover } from "@mantine/hooks";
import { observer } from "mobx-react";
import { FC, ReactNode, useContext } from "react";
import { ArcherElement } from "react-archer";
import { ParametersContext } from "../../juceIntegration/JuceIntegration";

interface ModulatorComponentProps {
  title: string;
  children: ReactNode;
  enabledParamId: string;
}
const ModulatorComponent: FC<ModulatorComponentProps> = observer(
  ({ title, children, enabledParamId }) => {
    const parameters = useContext(ParametersContext)!;

    return (
      <Card shadow="lg" p="sm" radius="md" pos="relative">
        <Stack align="stretch" spacing="sm">
          <Text weight={500} ta="center">
            {title}
          </Text>
          <CloseButton
            pos="absolute"
            right={10}
            onClick={() => ((parameters as any)[enabledParamId].value = false)}
          />

          {children}
        </Stack>
      </Card>
    );
  }
);

interface Props {
  id: string;
  title: string;
  children: ReactNode;
  enabledParamId: string;
}

export const Modulator: FC<Props> = observer(
  ({ id, title, children, enabledParamId }) => {
    const parameters = useContext(ParametersContext)!;

    const { hovered, ref } = useHover();

    const targets: { paramId: string; value: number }[] = [];
    for (let parameterId of Object.keys(parameters)) {
      const value = (parameters as any)[parameterId].value;
      if (parameterId.startsWith(`modAmount_${id}_`) && value !== 0) {
        targets.push({
          paramId: parameterId.replace(`modAmount_${id}_`, ""),
          value,
        });
      }
    }

    return (
      <Box ref={ref} pos="relative" style={{ flexShrink: 0 }}>
        <ModulatorComponent
          title={title}
          children={children}
          enabledParamId={enabledParamId}
        />
        {hovered && (
          <ArcherElement
            id={id}
            relations={targets.map(({ paramId, value }) => ({
              targetId: paramId,
              targetAnchor: "right",
              sourceAnchor: "left",
              label: (
                <Box bg="white" style={{ borderRadius: 4 }}>
                  <Text>
                    {(value * 100).toLocaleString(undefined, {
                      minimumFractionDigits: 0,
                    }) + "%"}
                  </Text>
                </Box>
              ),
            }))}
          >
            <Box
              pos="absolute"
              left={0}
              right={0}
              top={0}
              bottom={0}
              style={{ pointerEvents: "none" }}
            />
          </ArcherElement>
        )}
      </Box>
    );
  }
);
